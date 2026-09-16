// radio/main.cpp -- programa da issue #15, rodado na bancada pela #17 e pela #18.
//
// Mede as duas coisas que decidem entre o Plano A e o Plano B (R19, secao 8 da
// spec): a memoria livre no instante da conexao segura (teste 7d, R13a) e se a
// pagina do celular sobrevive ao pulo de canal quando o aparelho conecta no
// hotspot (teste 7c). Sem leitor de cracha, sem LED, sem buzzer, sem banco.
//
// ---------------------------------------------------------------------------
// O QUE ESTE PROGRAMA NAO E
//
// A pagina de login e de mentira: aceita qualquer senha e devolve um cookie. E
// andaime deste teste, e por isso mora embutida aqui, e nao em data/ como manda
// o R10 para o portal de verdade (a #20). Um uploadfs deste programa tambem
// sobrescreveria o portal na particao unica (R41). Mesmo de mentira, a senha vai
// por POST e nunca no endereco (R25), para o andaime nao ensinar o desenho errado.
// ---------------------------------------------------------------------------
//
// DOIS GATILHOS, UMA MEDICAO
//
//   botao "Conectar no hotspot agora", na pagina do celular, so para quem fez
//   login -- e o caminho da #18, com o celular do professor na rede do aparelho;
//
//   m no monitor serial -- e o caminho da #17, em que o unico celular e o
//   hotspot e ninguem faz login.
//
// O login sozinho nao mexe no radio. Os dois gatilhos chamam a mesma medicao.
//
// POR QUE O HOTSPOT SO CONECTA NO GATILHO
//
// O modo e WIFI_AP_STA desde o boot, mas o STA fica parado. Se conectasse no
// boot, a rede do aparelho ja nasceria no canal do hotspot, o celular entraria
// depois e o pulo de canal nunca aconteceria: o 7c nao mediria nada. Pela mesma
// razao, o boot procura o hotspot e sobe a rede do aparelho num canal distante
// do dele -- por isso o hotspot tem que estar ligado antes da placa. Depois da
// primeira conexao os dois ficam no mesmo canal, e repetir o 7c exige EN.
//
// A MEDICAO
//
// A lista de 40 alunos e um bloco reservado no heap, e nao um array global: o
// global iria para a .bss, fora do heap, e a medicao sairia folgada.
//
// So o handshake, contra o host do SUPABASE_URL (R13, setInsecure()): o criterio
// do 7d e o instante do connect(), e depois dele os buffers do mbedTLS ja estao
// alocados. Nao chama o rpc/roster, e por isso a chave publicavel nao e usada.
//
// Uma tentativa por gatilho, sem retry: conexao segura que falha vaza memoria
// (R13b), e o numero seguinte sairia contaminado. Depois de uma falha, so EN.
//
// 1 KB = 1024 bytes. As faixas sao as do secao 8: abaixo de 40 KB reprova; de
// 40 a 45 KB e suspeito, porque sem celular na rede do aparelho o numero sai
// otimista (R26), e quem decide e a #18 (R31); acima de 45 KB passa. O serial
// imprime os bytes tambem, para ninguem precisar reconverter.
//
// As frases do monitor serial sao o contrato com o roteiro da #16: mudou aqui,
// muda la.
//
// Loop sem delay(), como sempre (secao 5 da spec).

#include <Arduino.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <esp_wifi.h>
#include <secrets.h>

const size_t ALUNOS_NA_LISTA = 40;
const unsigned long ESPERA_DO_HOTSPOT_MS = 20000;
const byte PORTA_DNS = 53;
const IPAddress ENDERECO_DO_APARELHO(192, 168, 4, 1);

struct Aluno {
  char matricula[16];
  char nome[64];
  char uid[15];
};

DNSServer dns;
WebServer server(80);

Aluno *lista = nullptr;
String tokenDaSessao;

bool pedidoDoBotao = false;
bool conectando = false;
bool medicaoFalhou = false;
unsigned long inicioDaConexao = 0;
uint8_t canalAntesDaConexao = 0;
unsigned long saiuEm = 0;

const char PAGINA_LOGIN[] PROGMEM = R"HTML(<!doctype html>
<html lang="pt-BR"><head><meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Presente! - teste de rádio</title>
<style>body{font-family:sans-serif;margin:2em;max-width:24em}input,button{display:block;width:100%;font-size:1.2em;margin:.5em 0;padding:.4em}</style>
</head><body>
<h1>Teste de rádio</h1>
<p>Login de mentira: qualquer usuário e senha servem.</p>
<form method="post" action="/login">
<input name="usuario" placeholder="usuário">
<input name="senha" type="password" placeholder="senha">
<button>Entrar</button>
</form>
</body></html>)HTML";

const char PAGINA_PAINEL[] PROGMEM = R"HTML(<!doctype html>
<html lang="pt-BR"><head><meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Presente! - painel do teste</title>
<style>
body{font-family:sans-serif;margin:1.5em;max-width:26em}
#estado{font-size:2em;font-weight:bold;padding:1em .5em;text-align:center;color:#fff;background:#777;border-radius:.3em}
#botao{font-size:1.2em;width:100%;padding:.8em;margin-top:1.5em}
#botao:disabled{background:#ccc;color:#555}
</style>
</head><body>
<div id="estado">AGUARDANDO</div>
<p id="idade">Nenhuma resposta ainda.</p>
<button id="botao">Conectar no hotspot agora</button>
<p id="aviso"></p>
<script>
var ultima = 0;
function pedir(metodo, rota) {
  var controle = new AbortController();
  var limite = setTimeout(function () { controle.abort(); }, 2000);
  return fetch(rota, { method: metodo, signal: controle.signal })
    .finally(function () { clearTimeout(limite); });
}
function mostrar(texto, cor) {
  var estado = document.getElementById('estado');
  estado.textContent = texto;
  estado.style.background = cor;
}
function conferir() {
  pedir('GET', '/sessao').then(function (r) {
    ultima = Date.now();
    if (r.status === 200) { mostrar('LOGIN AINDA VALE', '#1a7f37'); }
    else { mostrar('PEDIU LOGIN DE NOVO', '#d9730d'); }
  }).catch(function () {
    mostrar('SEM RESPOSTA DO APARELHO', '#c62828');
  });
  if (ultima) {
    document.getElementById('idade').textContent =
      'Última resposta há ' + Math.floor((Date.now() - ultima) / 1000) + ' s';
  }
}
document.getElementById('botao').onclick = function () {
  var botao = this;
  var aviso = document.getElementById('aviso');
  botao.disabled = true;
  pedir('POST', '/conectar').then(function (r) {
    if (r.status === 200) {
      aviso.textContent = 'Pedido enviado: acompanhe no monitor serial';
      return;
    }
    aviso.textContent = 'O aparelho recusou o pedido: faça login de novo.';
    botao.disabled = false;
  }).catch(function () {
    aviso.textContent = 'O aparelho não respondeu: tente de novo.';
    botao.disabled = false;
  });
};
setInterval(conferir, 1000);
conferir();
</script>
</body></html>)HTML";

float emKb(uint32_t bytes) {
  return bytes / 1024.0f;
}

uint8_t canalAtual() {
  uint8_t primario = 0;
  wifi_second_chan_t secundario;
  esp_wifi_get_channel(&primario, &secundario);
  return primario;
}

bool sessaoValida() {
  if (tokenDaSessao.isEmpty() || !server.hasHeader("Cookie")) {
    return false;
  }
  return server.header("Cookie").indexOf("sessao=" + tokenDaSessao) >= 0;
}

void servirLogin() {
  server.send_P(200, "text/html; charset=utf-8", PAGINA_LOGIN);
}

void aceitarLogin() {
  char token[17];
  snprintf(token, sizeof(token), "%08x%08x", esp_random(), esp_random());
  tokenDaSessao = token;

  server.sendHeader("Set-Cookie", "sessao=" + tokenDaSessao + "; HttpOnly; Path=/");
  server.sendHeader("Location", "/painel");
  server.send(303);
  Serial.println("Login de mentira aceito.");
}

void servirPainel() {
  server.send_P(200, "text/html; charset=utf-8", PAGINA_PAINEL);
}

void responderSessao() {
  server.send(sessaoValida() ? 200 : 401, "text/plain", "");
}

void pedirConexao() {
  if (!sessaoValida()) {
    server.send(401, "text/plain", "");
    return;
  }
  // A resposta sai antes de o radio mexer: o loop() so le a flag na volta.
  server.send(200, "text/plain", "");
  pedidoDoBotao = true;
}

void mandarParaOPortal() {
  // Qualquer endereco volta para o login. E o que faz o celular abrir sozinho a
  // janelinha do portal cativo, que a #18 observa.
  server.sendHeader("Location", "http://192.168.4.1/");
  server.send(302);
}

void aoMudarARede(arduino_event_id_t evento) {
  const unsigned long segundos = millis() / 1000;

  if (evento == ARDUINO_EVENT_WIFI_AP_STACONNECTED) {
    Serial.printf("Celular entrou na rede do aparelho (%d conectado(s)) - %lu s desde o boot\n",
                  WiFi.softAPgetStationNum(), segundos);
    if (saiuEm != 0) {
      // E o "se cair, volta em <5 s" do 7c, visto do lado do aparelho.
      Serial.printf("Celular voltou depois de %.1f s fora\n", (millis() - saiuEm) / 1000.0f);
      saiuEm = 0;
    }
    return;
  }

  if (evento == ARDUINO_EVENT_WIFI_AP_STADISCONNECTED) {
    saiuEm = millis();
    Serial.printf("Celular saiu da rede do aparelho (%d conectado(s)) - %lu s desde o boot\n",
                  WiFi.softAPgetStationNum(), segundos);
  }
}

uint8_t escolherCanalDoAparelho() {
  const int redes = WiFi.scanNetworks();
  int canalDoHotspot = 0;
  for (int i = 0; i < redes; i++) {
    if (WiFi.SSID(i) == STA_SSID) {
      canalDoHotspot = WiFi.channel(i);
      break;
    }
  }
  WiFi.scanDelete();

  if (canalDoHotspot == 0) {
    Serial.printf("Hotspot %s NAO encontrado. Ligue o hotspot antes de ligar a placa, senao o teste 7c pode nao valer.\n",
                  STA_SSID);
    return 1;
  }

  Serial.printf("Hotspot %s encontrado no canal %d.\n", STA_SSID, canalDoHotspot);
  return canalDoHotspot <= 5 ? 11 : 1;
}

void imprimirMemoria(const char *rotulo, uint32_t bytes) {
  Serial.printf("%s: %u bytes (%.1f KB)", rotulo, bytes, emKb(bytes));
}

void abrirConexaoSegura() {
  String host = SUPABASE_URL;
  if (host.startsWith("https://")) {
    host.remove(0, 8);
  }
  const int barra = host.indexOf('/');
  if (barra >= 0) {
    host.remove(barra);
  }

  WiFiClientSecure cliente;
  cliente.setInsecure();

  // O ANTES e a ultima coisa antes do connect(): e o numero do criterio do 7d.
  const int celulares = WiFi.softAPgetStationNum();
  const uint32_t antes = ESP.getFreeHeap();
  imprimirMemoria("MEMORIA ANTES da conexao segura", antes);
  Serial.printf(" - celulares na rede do aparelho: %d\n", celulares);

  // Com o nome, e nao com o IP, o connect() manda o SNI que o host exige.
  if (!cliente.connect(host.c_str(), 443)) {
    char erro[100];
    const int codigo = cliente.lastError(erro, sizeof(erro));
    Serial.printf("A conexao segura FALHOU (%d: %s). A tentativa que falha vaza memoria: aperte EN antes de medir de novo.\n",
                  codigo, erro);
    medicaoFalhou = true;
    return;
  }

  Serial.printf("Conexao segura aberta com %s.\n", host.c_str());
  imprimirMemoria("MEMORIA DURANTE a conexao segura", ESP.getFreeHeap());
  Serial.println();
  imprimirMemoria("Menor memoria livre desde o boot", ESP.getMinFreeHeap());
  Serial.println();

  const float kb = emKb(antes);
  Serial.printf("RESULTADO: %.1f KB - ", kb);
  if (kb < 40) {
    Serial.println("ABAIXO DE 40 KB - o Plano A reprova");
  } else if (kb <= 45) {
    Serial.println("ENTRE 40 E 45 KB - SUSPEITO, quem decide e a #18");
  } else {
    Serial.println("ACIMA DE 45 KB - passa");
  }
  if (celulares == 0) {
    Serial.println("  (ninguem conectado na rede do aparelho: o numero sai otimista, R26)");
  }

  cliente.stop();
}

void medir() {
  if (conectando) {
    return;
  }
  if (medicaoFalhou) {
    Serial.println("Uma medicao anterior falhou: aperte EN antes de medir de novo.");
    return;
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Ja conectado no hotspot: esta medicao nao tem pulo de canal. Para repetir o teste 7c, aperte EN e faca login de novo.");
    abrirConexaoSegura();
    return;
  }

  Serial.printf("Conectando no hotspot %s...\n", STA_SSID);
  canalAntesDaConexao = canalAtual();
  inicioDaConexao = millis();
  conectando = true;
  WiFi.begin(STA_SSID, STA_PASSWORD);
}

// A espera pelo hotspot nao bloqueia: o loop() segue atendendo o DNS e as
// paginas, senao a pagina do celular pareceria cair por culpa do programa, e nao
// do radio.
void acompanharConexao() {
  if (!conectando) {
    return;
  }

  if (WiFi.status() == WL_CONNECTED) {
    conectando = false;
    const uint8_t canalAgora = canalAtual();
    Serial.printf("Conectado no hotspot em %.1f s. Canal da rede do aparelho: antes %d, agora %d.\n",
                  (millis() - inicioDaConexao) / 1000.0f, canalAntesDaConexao, canalAgora);
    if (canalAgora == canalAntesDaConexao) {
      Serial.printf("O PULO DE CANAL NAO ACONTECEU: os dois ja estavam no canal %d. O teste 7c nao vale nesta rodada.\n",
                    canalAgora);
    }
    abrirConexaoSegura();
    return;
  }

  if (millis() - inicioDaConexao >= ESPERA_DO_HOTSPOT_MS) {
    conectando = false;
    WiFi.disconnect();
    Serial.println("Nao conectei no hotspot em 20 s. Confira nome, senha e se ele esta em 2,4 GHz.");
  }
}

void atenderComando() {
  if (!Serial.available()) {
    return;
  }
  const char comando = Serial.read();
  if (comando == 'm' || comando == 'M') {
    Serial.println("Comando m recebido.");
    medir();
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("Presente! - teste de radio (issues #17 e #18)");

  WiFi.persistent(false);
  WiFi.mode(WIFI_AP_STA);
  const uint8_t canal = escolherCanalDoAparelho();

  WiFi.softAP(AP_SSID, AP_PASSWORD, canal);
  WiFi.onEvent(aoMudarARede);
  Serial.printf("Rede do aparelho no ar: %s, canal %d, endereco 192.168.4.1\n", AP_SSID, canalAtual());

  dns.start(PORTA_DNS, "*", ENDERECO_DO_APARELHO);

  const char *cabecalhos[] = {"Cookie"};
  server.collectHeaders(cabecalhos, 1);
  server.on("/", HTTP_GET, servirLogin);
  server.on("/login", HTTP_POST, aceitarLogin);
  server.on("/painel", HTTP_GET, servirPainel);
  server.on("/sessao", HTTP_GET, responderSessao);
  server.on("/conectar", HTTP_POST, pedirConexao);
  server.onNotFound(mandarParaOPortal);
  server.begin();

  const size_t tamanho = ALUNOS_NA_LISTA * sizeof(Aluno);
  lista = static_cast<Aluno *>(malloc(tamanho));
  if (lista == nullptr) {
    Serial.println("Nao consegui reservar a lista de 40 alunos: a medicao nao vale.");
    while (true) {
      yield();
    }
  }
  memset(lista, 'x', tamanho);
  Serial.printf("Lista de 40 alunos reservada na memoria: %u bytes\n", tamanho);

  Serial.println("Pronto. Faca login pelo celular na rede do aparelho e aperte o botao, ou digite m aqui para medir sem celular conectado.");
}

void loop() {
  dns.processNextRequest();
  server.handleClient();
  atenderComando();

  if (pedidoDoBotao) {
    pedidoDoBotao = false;
    Serial.println("Botao apertado no celular.");
    medir();
  }

  acompanharConexao();
}
