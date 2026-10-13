// presente/portal.cpp -- pacotes 1 e 2 do firmware (#41, #43): o portal dentro
// do aparelho.
//
// Das nove rotas do secao 6.1, os pacotes 1 e 2 tem tres: POST /api/login,
// GET /api/estado e POST /api/seguir-offline. Qualquer outra rota de /api/, e
// metodo errado nessas tres, recebe 404 ROTA_DESCONHECIDA. As seis que faltam
// chegam nos pacotes seguintes: POST /api/iniciar, POST /api/manual e
// GET /api/presencas no pacote 3 (#45); GET /api/relatorio, POST /api/enviar e
// POST /api/encerrar no pacote 4 (#47).
//
// A BUSCA DA TURMA (#43)
//
// Pelo secao 6.1, o login em AGUARDANDO_LOGIN responde e entao busca a turma:
// o estado passa a SINCRONIZANDO, com o azul lento, e volta a AGUARDANDO_INICIAR
// quando o net:: sai do hotspot, com a lista (online) ou sem ela (offline, R3).
// O seguir-offline abandona a busca em curso.
//
// FORA DE /api/ (R67)
//
// O uploadfs grava a pasta data/ inteira na placa, inclusive o professores.json,
// com o salt e o hash de cada professor. Por isso o aparelho so serve arquivo
// .html, .js ou .css, e so se ele existe. Todo o resto, existindo ou nao, vai
// com 302 para a pagina de login: e o que abre a janelinha do portal cativo no
// iPhone (/hotspot-detect.html) e no Android (/generate_204), e o que impede o
// professores.json de sair pela rede. Sem serveStatic, que serviria qualquer
// arquivo.

#include "portal.h"

#include <LittleFS.h>
#include <WebServer.h>
#include <config.h>
#include <feedback.h>
#include <mbedtls/sha256.h>

#include "net.h"

namespace portal {
namespace {

enum class Estado { AGUARDANDO_LOGIN, SINCRONIZANDO, AGUARDANDO_INICIAR };

WebServer server(80);

const JsonDocument* contas = nullptr;
bool comPortal = false;

Estado estado = Estado::AGUARDANDO_LOGIN;
int professorAtual = -1;  // posicao no professores.json; -1 antes do primeiro login

// Um token so, criado a cada login e guardado so na memoria (R62): o login
// seguinte troca o token, e o reinicio o perde.
char token[17] = "";

void responderJson(int status, const char* corpo) {
  server.send(status, "application/json", corpo);
}

void responderErro(int status, const char* codigo) {
  char corpo[48];
  snprintf(corpo, sizeof(corpo), "{\"erro\":\"%s\"}", codigo);
  responderJson(status, corpo);
}

bool comLogin() {
  if (token[0] == '\0' || !server.hasHeader("Cookie")) {
    return false;
  }
  char procurado[24];
  snprintf(procurado, sizeof(procurado), "sessao=%s", token);
  return server.header("Cookie").indexOf(procurado) >= 0;
}

// O hash e o SHA-256 do texto do salt seguido da senha, em hexadecimal
// minusculo -- o mesmo calculo do ferramentas/senha-professor.ps1. A comparacao
// percorre os 64 caracteres inteiros, sem sair no primeiro diferente.
bool senhaConfere(const char* salt, const char* senha, const char* hashGuardado) {
  if (salt == nullptr || hashGuardado == nullptr || strlen(hashGuardado) != 64) {
    return false;
  }

  char texto[300];
  const int tamanho = snprintf(texto, sizeof(texto), "%s%s", salt, senha);
  if (tamanho < 0 || tamanho >= static_cast<int>(sizeof(texto))) {
    return false;
  }

  unsigned char resumo[32];
  if (mbedtls_sha256_ret(reinterpret_cast<const unsigned char*>(texto), tamanho, resumo, 0) != 0) {
    return false;
  }

  char calculado[65];
  for (int i = 0; i < 32; i++) {
    snprintf(calculado + 2 * i, 3, "%02x", resumo[i]);
  }

  unsigned char diferenca = 0;
  for (int i = 0; i < 64; i++) {
    diferenca |= calculado[i] ^ hashGuardado[i];
  }
  return diferenca == 0;
}

void aoLogin() {
  StaticJsonDocument<256> corpo;
  if (deserializeJson(corpo, server.arg("plain")) != DeserializationError::Ok) {
    responderErro(401, "CREDENCIAL_INVALIDA");
    return;
  }

  // A hora vem no corpo e e ignorada: o relogio e do pacote 3, e a hora do
  // login, do pacote 5 (R62, item 3).
  const char* usuario = corpo["usuario"];
  const char* senha = corpo["senha"];
  if (usuario == nullptr || senha == nullptr) {
    responderErro(401, "CREDENCIAL_INVALIDA");
    return;
  }

  int encontrado = -1;
  int posicao = 0;
  for (JsonObjectConst conta : contas->as<JsonArrayConst>()) {
    const char* esse = conta["usuario"];
    if (esse != nullptr && strcmp(esse, usuario) == 0) {
      if (senhaConfere(conta["salt"], senha, conta["hash"])) {
        encontrado = posicao;
      }
      break;
    }
    posicao++;
  }
  if (encontrado < 0) {
    Serial.println("Login recusado.");
    responderErro(401, "CREDENCIAL_INVALIDA");
    return;
  }

  snprintf(token, sizeof(token), "%08x%08x", esp_random(), esp_random());

  // Fora de AGUARDANDO_LOGIN, o login so devolve um token: estado e professor
  // ficam como estao, sem busca nova, e o professor volta a chamada em curso
  // (R49).
  const bool buscar = estado == Estado::AGUARDANDO_LOGIN;
  if (buscar) {
    estado = Estado::SINCRONIZANDO;
    professorAtual = encontrado;
  }
  Serial.printf("Login aceito: %s.\n", usuario);

  char cookie[48];
  snprintf(cookie, sizeof(cookie), "sessao=%s; HttpOnly; Path=/", token);
  server.sendHeader("Set-Cookie", cookie);
  responderJson(200, "{\"ok\":true}");

  // Responde e entao busca (secao 6.1).
  if (buscar) {
    net::comecarBusca();
  }
}

void aoEstado() {
  StaticJsonDocument<512> doc;

  if (!comLogin()) {
    doc["autenticado"] = false;
  } else {
    JsonObjectConst professor = (*contas)[professorAtual];
    const bool sincronizando = estado == Estado::SINCRONIZANDO;
    doc["autenticado"] = true;
    doc["sincronizando"] = sincronizando;
    doc["modo"] = "offline";
    doc["roster_carregado"] = false;
    JsonObject quem = doc.createNestedObject("professor");
    // const char* aponta para o texto do documento global, sem copia-lo aqui.
    quem["id"] = professor["id"].as<const char*>();
    quem["nome"] = professor["nome"].as<const char*>();
    doc["turma"] = TURMA;
    doc["disciplina"] = DISCIPLINA;
    doc["total_alunos"] = nullptr;
    doc["sessao"] = nullptr;
    doc["ultimo"] = nullptr;
    doc["n_presentes"] = nullptr;
    doc["n_eventos"] = 0;
    doc["versao"] = 0;

    // Atribuir a uma chave que ja existe troca o valor no lugar.
    if (sincronizando) {
      doc["modo"] = nullptr;  // null enquanto a busca nao termina
    } else if (net::comLista()) {
      doc["modo"] = "online";
      doc["roster_carregado"] = true;
      doc["total_alunos"] = net::totalDeAlunos();
      doc["n_presentes"] = 0;
      doc["versao"] = 1;  // sobe quando a lista chega (secao 6.1)
    }
  }

  char saida[512];
  serializeJson(doc, saida, sizeof(saida));
  responderJson(200, saida);
}

// A primeira rota que exige o cookie (secao 6.1).
void aoSeguirOffline() {
  if (!comLogin()) {
    responderErro(401, "SEM_LOGIN");
    return;
  }
  // Durante a conexao segura este pedido espera o fim dela, porque o loop()
  // esta parado. O atender() passa a AGUARDANDO_INICIAR antes do pedido seguinte.
  net::abandonar();
  responderJson(200, net::comLista() ? "{\"ok\":true,\"modo\":\"online\"}"
                                     : "{\"ok\":true,\"modo\":\"offline\"}");
}

const char* tipoDoArquivo(const String& caminho) {
  if (caminho.endsWith(".html")) {
    return "text/html; charset=utf-8";
  }
  if (caminho.endsWith(".css")) {
    return "text/css";
  }
  if (caminho.endsWith(".js")) {
    return "application/javascript";
  }
  return nullptr;
}

// Sem cabecalho de cache: o streamFile nao manda Last-Modified, e o navegador
// nao guarda copia velha do portal depois de um uploadfs.
void servirArquivo(const char* caminho, const char* tipo) {
  File arquivo = LittleFS.open(caminho, "r");
  server.streamFile(arquivo, tipo);
  arquivo.close();
}

void mandarParaOLogin() {
  server.sendHeader("Location", String("http://") + ENDERECO_DO_APARELHO + "/");
  server.send(302);
}

// Tudo o que nao e POST /api/login, GET /api/estado nem POST /api/seguir-offline
// cai aqui.
void aoResto() {
  const String caminho = server.uri();

  if (caminho.startsWith("/api/")) {
    responderErro(404, "ROTA_DESCONHECIDA");
    return;
  }

  if (caminho == "/") {
    if (comPortal && LittleFS.exists("/index.html")) {
      servirArquivo("/index.html", "text/html; charset=utf-8");
      return;
    }
    // Sem o index.html, o 302 mandaria o / para ele mesmo, sem fim.
    server.send(503, "text/plain", "Portal nao gravado: falta o uploadfs.");
    return;
  }

  const char* tipo = tipoDoArquivo(caminho);
  if (tipo != nullptr && comPortal && LittleFS.exists(caminho)) {
    servirArquivo(caminho.c_str(), tipo);
    return;
  }
  mandarParaOLogin();
}

}  // namespace

void iniciar(const JsonDocument& professores, bool littlefsMontado) {
  contas = &professores;
  comPortal = littlefsMontado;

  const char* cabecalhos[] = {"Cookie"};
  server.collectHeaders(cabecalhos, 1);
  server.on("/api/login", HTTP_POST, aoLogin);
  server.on("/api/estado", HTTP_GET, aoEstado);
  server.on("/api/seguir-offline", HTTP_POST, aoSeguirOffline);
  server.onNotFound(aoResto);
  server.begin();
}

void atender() {
  // Antes do handleClient(): o pedido seguinte ja ve o estado do net::.
  if (estado == Estado::SINCRONIZANDO) {
    if (!net::buscando()) {
      estado = Estado::AGUARDANDO_INICIAR;
      feedback::acender(feedback::Cor::AZUL);
    } else if (!feedback::piscando()) {
      // O azul lento sem mexer na lib/feedback: uma piscada por vez, rearmada aqui.
      feedback::piscar(feedback::Cor::AZUL, 1, PISCADA_LENTA_MS, PISCADA_LENTA_MS);
    }
  }
  server.handleClient();
}

}  // namespace portal
