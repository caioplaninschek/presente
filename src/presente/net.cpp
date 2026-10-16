// presente/net.cpp -- pacotes 2, 4 e 5 do firmware (#43, #47, #49): a lista da
// turma pela internet, o envio do relatorio e a lista relida do disco na
// retomada.
//
// Depois do login, o aparelho entra no hotspot do professor (ou no de
// reserva), chama o rpc/roster do banco e guarda a lista na memoria e no
// /roster.json. A rede do aparelho fica no ar o tempo todo (WIFI_AP_STA, R45).
// O Enviar usa o mesmo caminho ate o banco (ver O ENVIO, abaixo).
//
// A ESPERA PELO HOTSPOT NAO TRAVA
//
// Como no acompanharConexao do programa de radio (#15): o WiFi.begin() volta na
// hora, e o atender() confere a cada volta do loop() se conectou ou se o tempo
// acabou. Enquanto isso, o DNS e o portal continuam respondendo. O principal
// espera ESPERA_DO_HOTSPOT_MS e a reserva, ESPERA_DA_RESERVA_MS; nome vazio no
// secrets.h se pula.
//
// UMA CONEXAO SEGURA POR BUSCA (R68)
//
// So a conexao segura e a leitura travam o loop(), cada uma com o seu limite no
// config.h. Sem segunda tentativa: conexao segura que falha vaza uns 4 KB
// (R13b), duas nao cabem nos 20 s da busca, e a busca que falha cai no modo
// offline, que e o caminho previsto (R3).
//
// Sem certificado (setInsecure(), R13) e com a chave so no cabecalho apikey,
// nunca como Bearer (R35). O useHTTP10(true) faz o banco responder sem
// pedacos (chunked), e o getStream() entrega o JSON puro direto ao
// deserializeJson(), sem copiar a resposta para uma String (R13c).
//
// LISTA VAZIA E FALHA (R68)
//
// Turma com o codigo errado, ou sem aluno ativo, volta do banco como [] com
// status 200. Tratada como lista, ela faria todo cracha cair em NAO_RECONHECIDO
// no pacote 3 (#45). Como falha, o aparelho segue offline, aceita todo cracha e
// o banco julga depois (R3).
//
// O APARELHO SAI DO HOTSPOT NO FIM (R68)
//
// Com ou sem a lista, WiFi.disconnect(). Se o hotspot sumisse no meio da aula,
// o ESP32 o procuraria de canal em canal, a rede do aparelho pularia junto e o
// celular do professor poderia cair. O Enviar (#47) entra de novo.
//
// O ENVIO (#47)
//
// O WebServer so responde ao fim do tratador, entao o Enviar trava o loop() pela
// tentativa inteira: os mesmos tentar(), tempos e conexao segura da busca, num
// laco sem delay() que chama o aoEsperar() de quem pediu (o portal mantem o
// azul lento, porque o net:: nao conhece o LED). Os quatro tempos somam 20 s, e
// o portal espera 25 s. Uma conexao segura so por Enviar (R13b). O relatorio
// vai como arquivo (Stream com tamanho), porque o HTTPClient nao manda corpo em
// pedacos e o relatorio nao cabe num documento fixo.
//
// Nao chegar ao banco (sem hotspot, sem DNS, sem conexao segura, sem resposta,
// ou o tempo que estoura depois do corpo) e SEM_INTERNET: tentar de novo e
// seguro, porque o banco responde ja_enviado ao repetido (HU-39). O banco que
// responde outro status, ou 200 sem um resultado conhecido, e BANCO_RECUSOU.
//
// Toda saida do enviar() passa por um ponto so: sai do hotspot, poe a fase em
// PARADA e apaga o /relatorio.json. Sem o encerrar() da busca, que diria que o
// aparelho segue offline. A fase que ficasse em PRINCIPAL ou RESERVA faria o
// atender() rodar a busca e apagar a lista da turma no meio da aula.
//
// O endereco do banco nunca sai no serial: e credencial (secrets.h), e o log vai
// colado em comentario de issue, num repositorio publico. As frases do monitor
// serial sao o contrato com o roteiro da #44: mudou aqui, muda la.

#include "net.h"

#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <LittleFS.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <config.h>
#include <esp_wifi.h>
#include <secrets.h>
#include <storage.h>

namespace net {
namespace {

enum class Fase { PARADA, PRINCIPAL, RESERVA };

// Global, fora da pilha do loop(), e fica na memoria para o pacote 3 (#45)
// conferir os crachas. 40 alunos ocupam uns 5 KB.
// shortcut: turma acima de uns 60 alunos nao cabe e cai no offline; aumentar o documento se a turma crescer
StaticJsonDocument<8192> lista;

Fase fase = Fase::PARADA;
bool carregada = false;
unsigned long inicioDaTentativa = 0;
uint8_t canalAntes = 0;

uint8_t canalAtual() {
  uint8_t primario = 0;
  wifi_second_chan_t secundario;
  esp_wifi_get_channel(&primario, &secundario);
  return primario;
}

const char* rotulo(Fase qual) {
  return qual == Fase::PRINCIPAL ? "principal" : "de reserva";
}

unsigned long espera(Fase qual) {
  return qual == Fase::PRINCIPAL ? ESPERA_DO_HOTSPOT_MS : ESPERA_DA_RESERVA_MS;
}

// Devolve false, sem tentar, se o nome desse hotspot esta vazio no secrets.h.
bool tentar(Fase qual) {
  const bool principal = qual == Fase::PRINCIPAL;
  const char* nome = principal ? STA_SSID : STA_SSID_FALLBACK;
  if (nome[0] == '\0') {
    Serial.printf("Hotspot %s vazio no secrets.h: pulado.\n", rotulo(qual));
    return false;
  }
  Serial.printf("Conectando no hotspot %s, %s (ate %lu s)...\n", rotulo(qual), nome,
                espera(qual) / 1000);
  // O retorno fica de fora (#43): toda recusa do core ja sai no serial como [E][WiFiSTA.cpp:<linha>],
  // e, com o pedido aceito, ele e o status() do momento, que pode ainda ser o da tentativa anterior.
  WiFi.begin(nome, principal ? STA_PASSWORD : STA_PASSWORD_FALLBACK);
  fase = qual;
  inicioDaTentativa = millis();
  return true;
}

void encerrar() {
  if (!carregada) {
    Serial.println("Sem a lista: o aparelho segue em modo offline (R3).");
  }
  WiFi.disconnect();
  fase = Fase::PARADA;
  Serial.println("Saiu do hotspot. A rede do aparelho continua no ar.");
}

enum class Espera { ESPERANDO, CONECTADO, DESISTIU };

// Um passo da espera pelo hotspot, o mesmo na busca e no envio: o principal e,
// sem ele, o de reserva.
Espera esperarHotspot() {
  if (WiFi.status() == WL_CONNECTED) {
    Serial.printf("Conectado no hotspot %s em %.1f s. Canal da rede do aparelho: antes %d, agora %d.\n",
                  rotulo(fase), (millis() - inicioDaTentativa) / 1000.0f, canalAntes, canalAtual());
    return Espera::CONECTADO;
  }
  if (millis() - inicioDaTentativa < espera(fase)) {
    return Espera::ESPERANDO;
  }
  Serial.printf("O hotspot %s nao respondeu em %lu s.\n", rotulo(fase), espera(fase) / 1000);
  WiFi.disconnect();
  if (fase == Fase::PRINCIPAL && tentar(Fase::RESERVA)) {
    return Espera::ESPERANDO;
  }
  Serial.println("Nenhum hotspot respondeu: confira nome, senha e se ele esta em 2,4 GHz (R13d).");
  return Espera::DESISTIU;
}

// A conexao segura da busca e do envio, ate a ultima coisa antes do pedido.
bool prepararPedido(WiFiClientSecure& cliente, HTTPClient& http, const char* rota) {
  cliente.setInsecure();
  static_assert(LIMITE_DA_CONEXAO_SEGURA_MS >= 1000 && LIMITE_DA_CONEXAO_SEGURA_MS % 1000 == 0,
                "o setHandshakeTimeout so aceita segundos inteiros");
  cliente.setHandshakeTimeout(LIMITE_DA_CONEXAO_SEGURA_MS / 1000);  // em segundos

  http.setConnectTimeout(LIMITE_DA_CONEXAO_SEGURA_MS);
  http.setTimeout(LIMITE_DA_RESPOSTA_MS);
  http.useHTTP10(true);
  if (!http.begin(cliente, String(SUPABASE_URL) + rota)) {
    Serial.println("O endereco do banco no secrets.h nao e um endereco https.");
    return false;
  }
  http.addHeader("apikey", SUPABASE_PUBLISHABLE_KEY);
  http.addHeader("Content-Type", "application/json");

  // A ultima coisa antes da conexao segura, no formato do programa de radio.
  const uint32_t antes = ESP.getFreeHeap();
  Serial.printf("MEMORIA ANTES da conexao segura: %u bytes (%.1f KB)\n", antes, antes / 1024.0f);
  return true;
}

// true, com o motivo no serial, quando o pedido nao chegou a uma resposta.
bool semResposta(int status, WiFiClientSecure& cliente) {
  if (status == HTTPC_ERROR_CONNECTION_REFUSED) {
    // Quando o DNS falha, o connect() devolve 0 sem gravar o erro, e o lastError()
    // nao escreve no buffer nem devolve codigo.
    char erro[100] = "nao achei o endereco do banco";
    const int codigo = cliente.lastError(erro, sizeof(erro));
    Serial.printf("A conexao segura FALHOU (%d: %s).\n", codigo, erro);
    if (codigo == 0) {
      Serial.println("O banco pode estar pausado (R60): reative no painel do Supabase.");
    }
    return true;
  }
  if (status < 0) {
    Serial.printf("O pedido ao banco FALHOU (%s).\n", HTTPClient::errorToString(status).c_str());
    return true;
  }
  return false;
}

// Trava o loop() pela conexao segura e pela leitura. Devolve true so com uma
// lista de verdade em `lista`; senao, diz o motivo no serial.
bool buscarNoBanco() {
  WiFiClientSecure cliente;
  HTTPClient http;
  if (!prepararPedido(cliente, http, "/rest/v1/rpc/roster")) {
    return false;
  }

  char corpo[48];
  snprintf(corpo, sizeof(corpo), "{\"turma\":\"%s\"}", TURMA);

  const int status = http.POST(reinterpret_cast<uint8_t*>(corpo), strlen(corpo));
  if (semResposta(status, cliente)) {
    return false;
  }
  if (status != 200) {
    Serial.printf("O banco respondeu %d, e nao 200.\n", status);
    return false;
  }

  const DeserializationError erro = deserializeJson(lista, http.getStream());
  http.end();
  if (erro == DeserializationError::NoMemory) {
    Serial.printf("A lista nao coube nos %u bytes do documento (NoMemory).\n", lista.capacity());
    return false;
  }
  if (erro) {
    Serial.printf("A resposta do banco nao e um JSON legivel (%s).\n", erro.c_str());
    return false;
  }
  const JsonArrayConst alunos = lista.as<JsonArrayConst>();
  if (alunos.isNull()) {
    Serial.println("A resposta do banco nao e uma lista.");
    return false;
  }
  if (alunos.size() == 0) {
    Serial.println("A lista da turma veio vazia: confira a TURMA no config.h e os alunos ativos no banco.");
    return false;
  }

  Serial.printf("Lista da turma carregada: %u alunos (%u bytes de %u)\n", alunos.size(),
                lista.memoryUsage(), lista.capacity());
  return true;
}

// O pedido do envio, com o hotspot ja conectado. Sai antes do enviar() apagar o
// arquivo, que assim ja esta fechado.
Envio mandarAoBanco(const char* caminho) {
  File arquivo = LittleFS.open(caminho, "r");
  if (!arquivo) {
    // O portal acabou de grava-lo: e defeito do disco, e o Compartilhar e o
    // conselho certo, como no BANCO_RECUSOU.
    Serial.printf("Nao consegui reabrir o %s.\n", caminho);
    return Envio::BANCO_RECUSOU;
  }

  WiFiClientSecure cliente;
  HTTPClient http;
  if (!prepararPedido(cliente, http, "/rest/v1/rpc/enviar_relatorio")) {
    return Envio::BANCO_RECUSOU;
  }

  Serial.printf("Mandando o relatorio: %u bytes.\n", arquivo.size());
  const int status = http.sendRequest("POST", &arquivo, arquivo.size());
  if (semResposta(status, cliente)) {
    return Envio::SEM_INTERNET;
  }
  if (status != 200) {
    Serial.printf("O banco respondeu %d, e nao 200.\n", status);
    return Envio::BANCO_RECUSOU;
  }

  StaticJsonDocument<192> resposta;
  const DeserializationError erro = deserializeJson(resposta, http.getStream());
  http.end();
  if (erro == DeserializationError::EmptyInput || erro == DeserializationError::IncompleteInput) {
    // O tempo estourou na leitura, depois de o banco receber o corpo.
    Serial.printf("A resposta do banco nao chegou inteira (%s).\n", erro.c_str());
    return Envio::SEM_INTERNET;
  }
  const char* resultado = resposta["resultado"] | "";
  if (!erro && strcmp(resultado, "gravado") == 0) {
    Serial.printf("Relatorio enviado: gravado, %d eventos (%d enriquecidos no banco).\n",
                  resposta["eventos"] | -1, resposta["enriquecidos_no_banco"] | -1);
    return Envio::GRAVADO;
  }
  if (!erro && strcmp(resultado, "ja_enviado") == 0) {
    Serial.println("Relatorio enviado: ja_enviado, o banco ja tinha esta chamada e nada se duplicou.");
    return Envio::JA_ENVIADO;
  }
  Serial.println("A resposta do banco nao traz gravado nem ja_enviado.");
  return Envio::BANCO_RECUSOU;
}

}  // namespace

void comecarBusca() {
  if (fase != Fase::PARADA) {
    return;
  }
  lista.clear();
  carregada = false;
  Serial.printf("Buscando a lista da turma %s.\n", TURMA);
  if (!storage::apagarRoster()) {
    Serial.println("Nao consegui apagar o /roster.json antigo.");
  }

  canalAntes = canalAtual();
  if (tentar(Fase::PRINCIPAL) || tentar(Fase::RESERVA)) {
    return;
  }
  Serial.println("Nenhum hotspot no secrets.h: STA_SSID e STA_SSID_FALLBACK vazios.");
  encerrar();
}

void atender() {
  if (fase == Fase::PARADA) {
    return;
  }

  const Espera passo = esperarHotspot();
  if (passo == Espera::ESPERANDO) {
    return;
  }
  if (passo == Espera::CONECTADO) {
    carregada = buscarNoBanco();
    if (!carregada) {
      lista.clear();
    } else if (storage::gravarRoster(lista)) {
      Serial.println("Lista gravada no /roster.json.");
    } else {
      Serial.println("Nao consegui gravar o /roster.json: o aparelho segue online, com a lista so na memoria.");
    }
  }
  encerrar();
}

bool buscando() {
  return fase != Fase::PARADA;
}

bool comLista() {
  return carregada;
}

size_t totalDeAlunos() {
  return carregada ? lista.as<JsonArrayConst>().size() : 0;
}

JsonArrayConst alunos() {
  return carregada ? lista.as<JsonArrayConst>() : JsonArrayConst();
}

void abandonar() {
  if (fase == Fase::PARADA) {
    return;
  }
  Serial.println("Busca abandonada: o professor escolheu seguir offline.");
  encerrar();
}

// Sem o encerrar(): a frase dele diria que o aparelho segue offline, e o
// Encerrar volta ao login.
void esquecerLista() {
  if (fase != Fase::PARADA) {
    WiFi.disconnect();
    fase = Fase::PARADA;
    Serial.println("Busca interrompida pelo Encerrar: saiu do hotspot.");
  }
  lista.clear();
  carregada = false;
  if (!storage::apagarRoster()) {
    Serial.println("Nao consegui apagar o /roster.json.");
  }
}

bool carregarLista() {
  lista.clear();
  carregada = false;
  const DeserializationError erro = storage::lerRoster(lista);
  if (erro) {
    Serial.printf("Nao consegui reler o %s (%s).\n", storage::CAMINHO_DO_ROSTER, erro.c_str());
    lista.clear();
    return false;
  }
  // A mesma regra da busca (R68): so uma lista de verdade conta.
  const JsonArrayConst alunos = lista.as<JsonArrayConst>();
  if (alunos.isNull() || alunos.size() == 0) {
    Serial.printf("O %s nao traz uma lista com alunos.\n", storage::CAMINHO_DO_ROSTER);
    lista.clear();
    return false;
  }
  carregada = true;
  Serial.printf("Lista da turma relida do disco: %u alunos.\n", alunos.size());
  return true;
}

Envio enviar(const char* caminho, void (*aoEsperar)()) {
  Serial.println("Enviando o relatorio ao banco.");
  canalAntes = canalAtual();
  Envio resultado = Envio::SEM_INTERNET;
  if (tentar(Fase::PRINCIPAL) || tentar(Fase::RESERVA)) {
    Espera passo;
    while ((passo = esperarHotspot()) == Espera::ESPERANDO) {
      aoEsperar();
    }
    if (passo == Espera::CONECTADO) {
      resultado = mandarAoBanco(caminho);
    }
  } else {
    Serial.println("Nenhum hotspot no secrets.h: STA_SSID e STA_SSID_FALLBACK vazios.");
  }

  // O ponto de saida unico, para todo desfecho (ver O ENVIO, no topo).
  WiFi.disconnect();
  fase = Fase::PARADA;
  if (!LittleFS.remove(caminho)) {
    Serial.printf("Nao consegui apagar o %s: o Encerrar tenta de novo.\n", caminho);
  }
  Serial.println("Envio terminado: saiu do hotspot. A lista da turma nao mudou.");
  return resultado;
}

}  // namespace net
