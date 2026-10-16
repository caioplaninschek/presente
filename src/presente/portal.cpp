// presente/portal.cpp -- pacotes 1 a 5 do firmware (#41, #43, #45, #47, #49):
// o portal dentro do aparelho.
//
// O firmware tem as nove rotas do secao 6.1: POST /api/login, GET /api/estado,
// POST /api/seguir-offline, POST /api/iniciar, POST /api/manual,
// GET /api/presencas e, do pacote 4 (#47), GET /api/relatorio,
// POST /api/enviar e POST /api/encerrar. Qualquer outra rota de /api/, e metodo
// errado nessas nove, recebe 404 ROTA_DESCONHECIDA.
//
// O RELATORIO, O ENVIAR E O ENCERRAR (#47)
//
// O relatorio sai em pedacos, escrito pelo relatorio.cpp, enquanto houver
// chamada iniciada, tambem depois do Enviar (R46). O Enviar grava
// {"relatorio": ...} no /relatorio.json e o manda pelo net::, que trava o loop()
// por uns 20 s, e o pior caso passa disso (R76). Durante a tentativa so o LED
// muda, para o azul lento do secao 5: o estado continua SESSAO_ABERTA, porque
// com SINCRONIZANDO o atender() leria o fim da busca e cairia em
// AGUARDANDO_INICIAR. No sucesso, a chamada fecha
// (CHAMADA_ENVIADA, azul fixo): o relatorio continua na tela, e o cracha recebe
// o sinal de NAO_RECONHECIDO sem passar pelo chamada::, que mudaria o ultimo do
// painel. Na falha, a chamada continua aberta, com o verde fixo. O fim do
// relatorio e a hora do comeco da tentativa que deu certo. O Encerrar vale em
// qualquer estado com login, ate durante a busca da turma: apaga a lista, a
// chamada e os arquivos dela, invalida o cookie e volta ao login. Responde
// sempre 200: a memoria ja esta limpa, e prender o professor no painel nao
// apagaria o arquivo que falhou.
//
// A RETOMADA (#49)
//
// Ao ligar, se o /sessao_atual.json traz uma chamada, ela volta em
// SESSAO_ABERTA, ou em CHAMADA_ENVIADA se ja tinha sido enviada, com o azul
// fixo e sem login: o cookie nao sobrevive ao reinicio (R62, item 2). O estado
// nunca e AGUARDANDO_LOGIN, porque o login nesse estado busca a turma, e a
// busca apaga o /roster.json no meio da aula. O leitor so volta a ser
// consultado no primeiro login do boot novo com hora valida, que acerta o
// relogio deste boot (saida 2 da #49); com a chamada aberta, o LED vai ao verde
// fixo. Ate la, o manual e o Enviar tambem recusam, porque o horario sairia
// errado. O painel e o relatorio continuam com o professor que iniciou a
// chamada, achado no professores.json pelo professorId do arquivo (R78).
//
// A CHAMADA (#45)
//
// O Iniciar abre a chamada (chamada.cpp) e passa a SESSAO_ABERTA, com o verde
// fixo. So nesse estado, e depois do Enviar, o leitor e consultado (R74): o
// cracha encostado antes do Iniciar nao tem sinal nenhum. A decisao do toque e do chamada::; aqui fica o
// sinal do secao 5 e a linha do serial, que leva o UID e a matricula, nunca o
// nome do aluno (R74).
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
#include <rfid.h>
#include <storage.h>

#include "chamada.h"
#include "clock.h"
#include "net.h"
#include "relatorio.h"

namespace portal {
namespace {

enum class Estado { AGUARDANDO_LOGIN, SINCRONIZANDO, AGUARDANDO_INICIAR, SESSAO_ABERTA, CHAMADA_ENVIADA };

// O alvo do secao 5 para o intervalo do encostar ate o sinal.
const unsigned long ALVO_DO_TOQUE_MS = 200;

WebServer server(80);

const JsonDocument* contas = nullptr;
bool comPortal = false;

Estado estado = Estado::AGUARDANDO_LOGIN;
int professorAtual = -1;  // posicao no professores.json; -1 antes do primeiro login
int quemEntrou = -1;      // o ultimo login: o lancado_por do manual (R78)

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

// Junta os bytes do serializeJson antes de manda-los ao celular: sem isto, cada
// caractere iria num write() proprio.
class Saida : public Print {
 public:
  size_t write(uint8_t c) override {
    pedaco[usados++] = c;
    if (usados == sizeof(pedaco)) {
      despejar();
    }
    return 1;
  }

  void despejar() {
    if (usados > 0) {
      server.sendContent(pedaco, usados);
      usados = 0;
    }
  }

 private:
  char pedaco[256];
  size_t usados = 0;
};

// Escreve o documento direto para o celular, com o tamanho medido antes: sem
// buffer de saida, a resposta nunca sai cortada.
void responderDocumento(int status, const JsonDocument& doc) {
  server.setContentLength(measureJson(doc));
  server.send(status, "application/json", "");
  Saida saida;
  serializeJson(doc, saida);
  saida.despejar();
}

const char* professorId() {
  return (*contas)[professorAtual]["id"];
}

// A posicao da conta com esse id no professores.json; -1 se nao ha.
int posicaoDaConta(const char* id) {
  if (id == nullptr) {
    return -1;
  }
  int posicao = 0;
  for (JsonObjectConst conta : contas->as<JsonArrayConst>()) {
    const char* esse = conta["id"];
    if (esse != nullptr && strcmp(esse, id) == 0) {
      return posicao;
    }
    posicao++;
  }
  return -1;
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

  // A hora vem no corpo e e ignorada: o relogio se acerta no Iniciar (#45), e a
  // hora do login e do pacote 5 (R62, item 3).
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

  // Fora de AGUARDANDO_LOGIN, o login so devolve um token: o estado fica como
  // esta, sem busca nova, e o professor volta a chamada em curso (R49). Outra
  // conta tambem entra (R78): a chamada iniciada continua de quem a iniciou, que
  // e o nome do painel, e o manual leva o id de quem entrou por ultimo; antes do
  // Iniciar, quem entra assume o painel.
  const bool buscar = estado == Estado::AGUARDANDO_LOGIN;
  if (buscar) {
    estado = Estado::SINCRONIZANDO;
  }
  // Na retomada cuja conta saiu do professores.json, nao ha quem mostrar no
  // painel: quem entra o assume, e a chamada continua no id do Iniciar (#49).
  if (!chamada::iniciada() || professorAtual < 0) {
    professorAtual = encontrado;
  }
  quemEntrou = encontrado;
  Serial.printf("Login aceito: %s.\n", usuario);

  // O primeiro login do boot que retomou a chamada traz a hora que faltava
  // (#49): com ela, o relogio deste boot se acerta, e o leitor volta.
  if (chamada::iniciada() && !relogio::acertadoNesteBoot()) {
    if (relogio::acertarNaRetomada(corpo["hora"])) {
      Serial.println("Hora do login: o relogio deste boot foi acertado, e o leitor volta a ler o cracha.");
      if (estado == Estado::SESSAO_ABERTA) {
        feedback::acender(feedback::Cor::VERDE);
      }
    } else {
      // DECISAO PENDENTE (#49): o secao 6.1 diz que a hora ausente ou fora do
      // formato nao impede o login, e o login entra. Mas, sem hora, este boot
      // nao grava nada: o leitor continua desligado, e o manual e o Enviar
      // recusam com NAO_GRAVADO, ate um login com hora valida.
      Serial.println("Login sem hora valida depois da retomada: o leitor continua desligado, e nada se grava ate um login com hora.");
    }
  }

  char cookie[48];
  snprintf(cookie, sizeof(cookie), "sessao=%s; HttpOnly; Path=/", token);
  server.sendHeader("Set-Cookie", cookie);
  responderJson(200, "{\"ok\":true}");

  // Responde e entao busca (secao 6.1).
  if (buscar) {
    net::comecarBusca();
  }
}

const char* codigoDoVeredito(chamada::Veredito veredito) {
  switch (veredito) {
    case chamada::Veredito::REGISTRADO:    return "REGISTRADO";
    case chamada::Veredito::JA_REGISTRADO: return "JA_REGISTRADO";
    default:                               return "NAO_RECONHECIDO";
  }
}

// Sobe a cada evento gravado e quando a lista da turma chega (secao 6.1); nao
// sobe com JA_REGISTRADO nem com NAO_RECONHECIDO, que nao gravam nada.
unsigned long versao() {
  return (net::comLista() ? 1 : 0) + chamada::eventos();
}

void aoEstado() {
  // O nome do aluno vem do banco, sem limite de tamanho: os textos entram como
  // const char*, sem copia, e o overflowed() avisa se o documento encheu.
  StaticJsonDocument<768> doc;
  char horaDoUltimo[relogio::TAMANHO_DA_HORA];

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
    doc["n_eventos"] = chamada::eventos();
    doc["versao"] = versao();

    // Atribuir a uma chave que ja existe troca o valor no lugar.
    if (sincronizando) {
      doc["modo"] = nullptr;  // null enquanto a busca nao termina
    } else if (net::comLista()) {
      doc["modo"] = "online";
      doc["roster_carregado"] = true;
      doc["total_alunos"] = net::totalDeAlunos();
      doc["n_presentes"] = chamada::presentes();
    }

    if (chamada::iniciada()) {
      JsonObject sessao = doc.createNestedObject("sessao");
      sessao["sessaoId"] = chamada::sessaoId();
      sessao["estado"] = chamada::aberta() ? "aberta" : "finalizada";
      sessao["inicio"] = chamada::inicio();
    }

    const chamada::Toque* toque = chamada::ultimo();
    if (toque != nullptr) {
      JsonObject ultimo = doc.createNestedObject("ultimo");
      ultimo["tipo"] = codigoDoVeredito(toque->tipo);
      ultimo["nome"] = toque->nome;
      ultimo["uid"] = static_cast<const char*>(toque->uid);
      if (relogio::formatar(toque->msDesdeBoot, horaDoUltimo, sizeof(horaDoUltimo))) {
        ultimo["timestamp"] = static_cast<const char*>(horaDoUltimo);
      } else {
        ultimo["timestamp"] = nullptr;
      }
    }
  }

  if (doc.overflowed()) {
    Serial.println("O /api/estado nao coube no documento de 768 bytes: a resposta saiu incompleta.");
  }
  responderDocumento(200, doc);
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

void aoIniciar() {
  if (!comLogin()) {
    responderErro(401, "SEM_LOGIN");
    return;
  }
  // Durante a busca o modo ainda nao existe; o SESSAO_JA_ABERTA nao serve aqui,
  // porque o portal o trata como sucesso (R74).
  if (estado == Estado::SINCRONIZANDO) {
    Serial.println("Iniciar recusado: AINDA_SINCRONIZANDO.");
    responderErro(400, "AINDA_SINCRONIZANDO");
    return;
  }

  // Corpo ilegivel ou sem a hora: hora nula, e o abrir() responde HORA_INVALIDA.
  StaticJsonDocument<128> corpo;
  deserializeJson(corpo, server.arg("plain"));
  switch (chamada::abrir(corpo["hora"], professorId(), millis())) {
    case chamada::Abertura::JA_ABERTA:
      Serial.println("Iniciar recusado: SESSAO_JA_ABERTA.");
      responderErro(400, "SESSAO_JA_ABERTA");
      return;
    case chamada::Abertura::HORA_INVALIDA:
      Serial.println("Iniciar recusado: HORA_INVALIDA.");
      responderErro(400, "HORA_INVALIDA");
      return;
    case chamada::Abertura::NAO_APAGOU:  // o motivo, o chamada.cpp ja disse (R77)
      Serial.println("Iniciar recusado: NAO_GRAVADO.");
      responderErro(400, "NAO_GRAVADO");
      return;
    case chamada::Abertura::ABERTA:
      break;
  }

  estado = Estado::SESSAO_ABERTA;
  feedback::acender(feedback::Cor::VERDE);

  StaticJsonDocument<192> resposta;
  resposta["ok"] = true;
  JsonObject sessao = resposta.createNestedObject("sessao");
  sessao["sessaoId"] = chamada::sessaoId();
  sessao["estado"] = "aberta";
  sessao["inicio"] = chamada::inicio();
  responderDocumento(200, resposta);
}

void aoManual() {
  if (!comLogin()) {
    responderErro(401, "SEM_LOGIN");
    return;
  }

  // Sem copia (char*): o motivo pode vir de qualquer tamanho, e o lancar() o
  // corta. Corpo ilegivel: matricula nula, e o lancar() responde MATRICULA_VAZIA.
  String texto = server.arg("plain");
  StaticJsonDocument<128> corpo;
  deserializeJson(corpo, texto.begin());

  const char* nome = nullptr;
  const chamada::Lancamento resultado =
      chamada::lancar(corpo["matricula"], corpo["motivo"], (*contas)[quemEntrou]["id"], relogio::agora(), nome);

  const char* codigo = nullptr;
  switch (resultado) {
    case chamada::Lancamento::ACEITO:
      Serial.printf("Manual aceito: %s\n", corpo["matricula"].as<const char*>());
      responderJson(200, "{\"ok\":true}");
      return;
    case chamada::Lancamento::JA_PRESENTE: {
      Serial.println("Manual recusado: JA_PRESENTE.");
      StaticJsonDocument<64> resposta;
      resposta["erro"] = "JA_PRESENTE";
      resposta["nome"] = nome;
      responderDocumento(400, resposta);
      return;
    }
    case chamada::Lancamento::SEM_SESSAO:      codigo = "SEM_SESSAO"; break;
    case chamada::Lancamento::MATRICULA_VAZIA: codigo = "MATRICULA_VAZIA"; break;
    case chamada::Lancamento::FORA_DA_TURMA:   codigo = "FORA_DA_TURMA"; break;
    case chamada::Lancamento::NAO_GRAVADO:     codigo = "NAO_GRAVADO"; break;  // R74
  }
  Serial.printf("Manual recusado: %s.\n", codigo);
  responderErro(400, codigo);
}

// O /api/presencas sai em pedacos, um item por vez (R74): com 40 nomes a
// resposta passa de 5 KB, e a R62 tirou as listas do /api/estado por isso.
// O percorrerEventos() pede um ponteiro de funcao, sem captura; dai a saida e a
// virgula guardadas aqui.
Saida* saidaDasPresencas = nullptr;
bool primeiroItem = true;

void escreverItem(const JsonDocument& item) {
  if (!primeiroItem) {
    saidaDasPresencas->write(',');
  }
  primeiroItem = false;
  serializeJson(item, *saidaDasPresencas);
}

void escreverEvento(JsonObjectConst evento) {
  char hora[relogio::TAMANHO_DA_HORA];
  StaticJsonDocument<256> item;
  const bool manual = strcmp(evento["origem"] | "", "manual") == 0;
  if (!net::comLista()) {
    item["uid"] = evento["uid"].as<const char*>();
  }
  item["matricula"] = evento["matricula"].as<const char*>();
  if (net::comLista()) {
    item["nome"] = evento["nome"].as<const char*>();
  }
  if (relogio::formatar(evento["ms_desde_boot"].as<unsigned long>(), hora, sizeof(hora))) {
    item["timestamp"] = static_cast<const char*>(hora);
  } else {
    item["timestamp"] = nullptr;
  }
  item["origem"] = evento["origem"].as<const char*>();
  if (net::comLista()) {
    item["lancado_por"] = manual ? evento["lancado_por"].as<const char*>() : nullptr;
    item["motivo"] = manual ? evento["motivo"].as<const char*>() : nullptr;
  }
  escreverItem(item);
}

void aoPresencas() {
  if (!comLogin()) {
    responderErro(401, "SEM_LOGIN");
    return;
  }

  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "application/json", "");
  Saida saida;
  saidaDasPresencas = &saida;

  saida.printf("{\"versao\":%lu,\"presentes\":[", versao());
  // Antes do Iniciar o arquivo nao se le: pode ser de outra chamada, ou das
  // bancadas (R74).
  primeiroItem = true;
  if (net::comLista() && chamada::iniciada()) {
    storage::percorrerEventos(escreverEvento);
  }

  saida.print("],\"faltantes\":[");
  primeiroItem = true;
  for (JsonObjectConst aluno : net::alunos()) {
    const char* matricula = aluno["matricula"];
    if (!chamada::presente(matricula)) {
      StaticJsonDocument<64> item;
      item["matricula"] = matricula;
      item["nome"] = aluno["nome"].as<const char*>();
      escreverItem(item);
    }
  }

  saida.print("],\"sem_nome\":[");
  primeiroItem = true;
  if (!net::comLista() && chamada::iniciada()) {
    storage::percorrerEventos(escreverEvento);
  }
  saida.print("]}");

  saida.despejar();
  saidaDasPresencas = nullptr;
  // O WebServer fecha a resposta em pedacos sozinho, ao fim do pedido.
}

void aoRelatorio() {
  if (!comLogin()) {
    responderErro(401, "SEM_LOGIN");
    return;
  }
  if (!chamada::iniciada()) {
    responderErro(400, "SEM_SESSAO");
    return;
  }

  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "application/json", "");
  Saida saida;
  relatorio::escrever(saida, chamada::fim());
  saida.despejar();
}

// O azul lento sem mexer na lib/feedback: uma piscada por vez, rearmada aqui.
void azulLento() {
  if (!feedback::piscando()) {
    feedback::piscar(feedback::Cor::AZUL, 1, PISCADA_LENTA_MS, PISCADA_LENTA_MS);
  }
}

// Com o loop() parado pelo Enviar, ninguem mais conduz o LED.
void esperandoOEnvio() {
  azulLento();
  feedback::atualizar();
}

bool gravarRelatorio(const char* fim) {
  File arquivo = LittleFS.open(storage::CAMINHO_DO_RELATORIO, "w");
  if (!arquivo) {
    return false;
  }
  const char abertura[] = "{\"relatorio\":";
  bool inteiro = arquivo.print(abertura) == strlen(abertura);
  inteiro = relatorio::escrever(arquivo, fim) && inteiro;
  inteiro = arquivo.print('}') == 1 && inteiro;
  arquivo.close();
  return inteiro;
}

void aoEnviar() {
  if (!comLogin()) {
    responderErro(401, "SEM_LOGIN");
    return;
  }
  if (!chamada::iniciada()) {
    responderErro(400, "SEM_SESSAO");
    return;
  }
  // A segunda trava contra o envio duplicado (HU-39): sem ir a internet. Pelo
  // chamada::, como o manual e o toque, e nao pelo estado do portal.
  if (!chamada::aberta()) {
    Serial.println("Enviar: a chamada ja foi enviada; ja_enviado sem ir a internet.");
    responderJson(200, "{\"resultado\":\"ja_enviado\"}");
    return;
  }

  // DECISAO PENDENTE (#49): depois da retomada, antes de um login com hora, o
  // fim sairia errado. O NAO_GRAVADO e o 503 que o portal ja conhece, com a
  // mensagem que manda usar o Compartilhar, e a chamada continua aberta.
  if (!relogio::acertadoNesteBoot()) {
    Serial.println("Enviar recusado: este boot ainda nao tem hora (falta o login com hora).");
    responderErro(503, "NAO_GRAVADO");
    return;
  }

  // O fim no relogio do Iniciar, tambem depois de uma retomada (#49).
  char fim[relogio::TAMANHO_DA_HORA];
  if (!relogio::formatar(relogio::agora(), fim, sizeof(fim)) || !gravarRelatorio(fim)) {
    LittleFS.remove(storage::CAMINHO_DO_RELATORIO);
    Serial.printf("Nao consegui gravar o %s: a chamada continua aberta.\n", storage::CAMINHO_DO_RELATORIO);
    responderErro(503, "NAO_GRAVADO");
    return;
  }

  // A cor de base fica azul: a piscada volta a ela, e o LED congela nela na
  // conexao segura.
  feedback::acender(feedback::Cor::AZUL);
  const net::Envio envio = net::enviar(storage::CAMINHO_DO_RELATORIO, esperandoOEnvio);

  if (envio == net::Envio::GRAVADO || envio == net::Envio::JA_ENVIADO) {
    chamada::fechar(fim);
    estado = Estado::CHAMADA_ENVIADA;
    feedback::acender(feedback::Cor::AZUL);
    Serial.printf("Chamada enviada: %s. O cracha nao registra mais.\n", chamada::sessaoId());
    responderJson(200, envio == net::Envio::GRAVADO ? "{\"resultado\":\"gravado\"}"
                                                    : "{\"resultado\":\"ja_enviado\"}");
    return;
  }

  feedback::acender(feedback::Cor::VERDE);
  const char* codigo = envio == net::Envio::SEM_INTERNET ? "SEM_INTERNET" : "BANCO_RECUSOU";
  Serial.printf("Enviar falhou: %s. A chamada continua aberta.\n", codigo);
  responderErro(503, codigo);
}

void aoEncerrar() {
  if (!comLogin()) {
    responderErro(401, "SEM_LOGIN");
    return;
  }

  net::esquecerLista();
  chamada::zerar();
  if (!storage::apagarChamada()) {
    Serial.println("Nao consegui apagar os arquivos da chamada: o proximo Iniciar tenta de novo.");
  }
  token[0] = '\0';  // o cookie deixa de valer
  professorAtual = -1;
  quemEntrou = -1;
  estado = Estado::AGUARDANDO_LOGIN;
  feedback::acender(feedback::Cor::AZUL);
  Serial.println("Encerrar: lista da turma, sessao e eventos apagados. De volta ao login.");
  responderJson(200, "{\"ok\":true}");
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

// Tudo o que nao e uma das nove rotas registradas no iniciar() cai aqui.
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

// O cracha, so em SESSAO_ABERTA. A janela de silencio de 5 s esta no
// lerTagNova() (R30), e a piscada volta sozinha ao verde fixo.
void lerCracha() {
  rfid::Uid uid;
  if (!rfid::lerTagNova(uid)) {
    return;
  }
  const unsigned long leituraVoltouEm = micros();

  const chamada::Veredito veredito = chamada::encostou(uid, relogio::agora());
  switch (veredito) {
    case chamada::Veredito::REGISTRADO:
      feedback::piscar(feedback::Cor::VERDE, 2, PISCADA_VERDE_ACESA_MS, PISCADA_VERDE_APAGADA_MS);
      feedback::bipar(BIP_DO_REGISTRO_MS);
      break;
    case chamada::Veredito::JA_REGISTRADO:
      feedback::piscar(feedback::Cor::VERDE, 1, PISCADA_VERDE_ACESA_MS, PISCADA_VERDE_APAGADA_MS);
      break;
    case chamada::Veredito::NAO_RECONHECIDO:
      feedback::piscar(feedback::Cor::VERMELHO, 1, VERMELHO_DO_NAO_RECONHECIDO_MS, 0);
      feedback::bipar(BUZZ_DO_NAO_RECONHECIDO_MS);
      break;
  }
  const unsigned long feedbackComecouEm = micros();

  // Os dois tempos do encostou (#26): "decidir" vai da leitura ao sinal, e
  // "toque->feedback" inclui a conversa com o leitor e e o que responde ao alvo
  // do secao 5, como piso (R38).
  const chamada::Toque* toque = chamada::ultimo();
  Serial.printf("%s  %s", toque->uid, codigoDoVeredito(veredito));
  if (toque->matricula != nullptr) {
    Serial.printf("  matricula %s", toque->matricula);
  }
  const unsigned long toqueAteFeedbackUs = feedbackComecouEm - rfid::inicioDaLeituraUs();
  Serial.printf("  decidir: %.1f ms  toque->feedback: %.1f ms",
                (feedbackComecouEm - leituraVoltouEm) / 1000.0, toqueAteFeedbackUs / 1000.0);
  if (toqueAteFeedbackUs > ALVO_DO_TOQUE_MS * 1000UL) {
    Serial.printf("  ACIMA DOS %lu ms", ALVO_DO_TOQUE_MS);
  }
  Serial.println();
}

// Depois do Enviar (R46): o sinal de NAO_RECONHECIDO, sem o chamada::encostou,
// e nada gravado. A janela de 5 s continua no lerTagNova().
void recusarCracha() {
  rfid::Uid uid;
  if (!rfid::lerTagNova(uid)) {
    return;
  }
  feedback::piscar(feedback::Cor::VERMELHO, 1, VERMELHO_DO_NAO_RECONHECIDO_MS, 0);
  feedback::bipar(BUZZ_DO_NAO_RECONHECIDO_MS);
  char texto[rfid::UID_TEXTO_TAMANHO];
  rfid::formatarUid(uid, texto, sizeof(texto));
  Serial.printf("%s  NAO_RECONHECIDO  chamada enviada: nada gravado\n", texto);
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
  server.on("/api/iniciar", HTTP_POST, aoIniciar);
  server.on("/api/manual", HTTP_POST, aoManual);
  server.on("/api/presencas", HTTP_GET, aoPresencas);
  server.on("/api/relatorio", HTTP_GET, aoRelatorio);
  server.on("/api/enviar", HTTP_POST, aoEnviar);
  server.on("/api/encerrar", HTTP_POST, aoEncerrar);
  server.onNotFound(aoResto);
  server.begin();
}

void atender() {
  // Antes do handleClient(): o pedido seguinte ja ve o estado do net::.
  if (estado == Estado::SINCRONIZANDO) {
    if (!net::buscando()) {
      estado = Estado::AGUARDANDO_INICIAR;
      feedback::acender(feedback::Cor::AZUL);
    } else {
      azulLento();
    }
  } else if (relogio::acertadoNesteBoot()) {
    // Depois da retomada, o leitor espera o login com hora (#49): ate la, nem
    // e consultado, como antes do Iniciar (R74).
    if (estado == Estado::SESSAO_ABERTA) {
      lerCracha();
    } else if (estado == Estado::CHAMADA_ENVIADA) {
      recusarCracha();
    }
  }
  server.handleClient();
}

void retomar() {
  StaticJsonDocument<768> sessao;
  const DeserializationError erro = storage::lerSessao(sessao);
  if (erro == DeserializationError::EmptyInput) {
    return;  // sem chamada para retomar: o login de sempre
  }
  if (erro) {
    Serial.printf("O %s nao pode ser lido (%s): nada a retomar.\n", storage::CAMINHO_DA_SESSAO, erro.c_str());
    return;
  }

  Serial.printf("Retomando a chamada do %s.\n", storage::CAMINHO_DA_SESSAO);
  const bool eraOnline = strcmp(sessao["modo"] | "", "online") == 0;
  if (eraOnline) {
    net::carregarLista();
  }

  // O id que a chamada guarda aponta o documento global das contas, e nao o
  // documento da sessao, que morre ao fim desta funcao.
  const int posicao = posicaoDaConta(sessao["professorId"]);
  const char* idDaConta = posicao >= 0 ? (*contas)[posicao]["id"].as<const char*>() : nullptr;

  switch (chamada::retomar(sessao.as<JsonObjectConst>(), idDaConta)) {
    case chamada::Retomada::NENHUMA:
      // A lista relida nao serve sem chamada; o login seguinte busca de novo.
      if (eraOnline) {
        net::esquecerLista();
      }
      return;
    case chamada::Retomada::ABERTA:
      estado = Estado::SESSAO_ABERTA;
      break;
    case chamada::Retomada::ENVIADA:
      estado = Estado::CHAMADA_ENVIADA;
      break;
  }

  professorAtual = posicao;  // -1 se a conta saiu: o primeiro login assume o painel
  quemEntrou = -1;
  feedback::acender(feedback::Cor::AZUL);
  Serial.println("O leitor espera o primeiro login com hora valida.");
}

}  // namespace portal
