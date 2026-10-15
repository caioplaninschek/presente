// presente/main.cpp -- o firmware de verdade (R51), pacotes 1 a 4 (#41, #43,
// #45, #47).
//
// O aparelho poe a rede propria no ar, o portal cativo abre a pagina gravada
// em data/, e o professor entra com a senha conferida por hash e recebe o
// cookie sessao. Depois do login, o aparelho busca a lista da turma pelo
// hotspot do professor (net.cpp). O Iniciar abre a chamada, e o cracha
// encostado e o lancamento manual a registram (chamada.cpp). O relatorio sai
// no Compartilhar e no Enviar, que o manda ao banco e fecha a chamada
// (relatorio.cpp, net.cpp); o Encerrar apaga tudo e volta ao login. A retomada
// depois de uma queda de energia chega no pacote 5 (#49).
//
// Os pinos, a turma e o endereco do aparelho vem do lib/config/config.h (R66);
// o nome e a senha da rede, do secrets.h.
//
// O LittleFS monta com begin(false): montagem que falha e erro a mostrar, nao
// particao a formatar, porque formatar apagaria o portal (R41). O aparelho
// segue com a rede no ar mesmo assim, e o serial diz o que falta gravar.
//
// Loop sem delay(), como sempre (secao 5 da spec).

#include <Arduino.h>
#include <ArduinoJson.h>
#include <DNSServer.h>
#include <LittleFS.h>
#include <WiFi.h>
#include <feedback.h>
#include <rfid.h>
#include <secrets.h>
#include <storage.h>

#include "net.h"
#include "portal.h"

const byte PORTA_DNS = 53;

DNSServer dns;

// Global, fora da pilha do loop(): dez contas no limite do senha-professor.ps1
// ocupam 3236 bytes (medido com a ArduinoJson 6.21.6).
StaticJsonDocument<4096> professores;

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("Presente! - firmware, pacote 4 (#47)");

  feedback::iniciar();
  feedback::acender(feedback::Cor::AZUL);  // AGUARDANDO_LOGIN

  // Sem o leitor, a chamada funciona so pelo lancamento manual.
  rfid::iniciar();
  const byte versao = rfid::versaoDoLeitor();
  if (rfid::leitorRespondeu()) {
    Serial.printf("RC522 respondeu - VersionReg = 0x%02X\n", versao);
  } else {
    Serial.printf("RC522 nao respondeu - VersionReg = 0x%02X\n", versao);
    Serial.println("  Confira 3,3 V no RC522, o RST no pino 22 e o SDA no pino 5.");
  }

  const bool montado = LittleFS.begin(false);
  if (!montado) {
    Serial.println("O LittleFS nao montou: grave o portal com uploadfs.");
  } else if (!LittleFS.exists("/index.html")) {
    Serial.println("Portal nao gravado: falta o uploadfs.");
  }

  if (montado) {
    const DeserializationError erro = storage::lerProfessores(professores);
    if (erro == DeserializationError::EmptyInput) {
      Serial.println("Sem professores.json: todo login vai ser recusado. Gere o arquivo com o ferramentas/senha-professor.ps1 e grave com uploadfs.");
    } else if (erro) {
      Serial.printf("O professores.json nao pode ser lido (%s): todo login vai ser recusado.\n", erro.c_str());
    }
    if (erro) {
      professores.clear();
    }
  }
  Serial.printf("Contas de professor lidas: %u (%u bytes de %u)\n",
                professores.as<JsonArrayConst>().size(), professores.memoryUsage(),
                professores.capacity());

  WiFi.persistent(false);
  WiFi.mode(WIFI_AP_STA);  // o STA so entra no hotspot durante a busca da turma (net.cpp)
  if (WiFi.softAP(AP_SSID, AP_PASSWORD)) {
    Serial.printf("Rede do aparelho no ar: %s, endereco %s\n", AP_SSID,
                  WiFi.softAPIP().toString().c_str());
  } else {
    Serial.println("A rede do aparelho NAO subiu: a senha da rede do aparelho (AP_PASSWORD, no secrets.h) precisa de 8 caracteres ou mais.");
  }

  dns.start(PORTA_DNS, "*", WiFi.softAPIP());
  portal::iniciar(professores, montado);

  Serial.println("Pronto. Conecte o celular na rede do aparelho.");
}

void loop() {
  dns.processNextRequest();
  net::atender();  // antes do portal, que ve o fim da busca na mesma volta
  portal::atender();
  feedback::atualizar();
}
