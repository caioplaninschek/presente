// presente/main.cpp -- o firmware de verdade (R51), pacote 1 (#41).
//
// O aparelho poe a rede propria no ar, o portal cativo abre a pagina gravada
// em data/, e o professor entra com a senha conferida por hash e recebe o
// cookie sessao. O leitor de cracha, a busca da turma e o envio chegam nos
// pacotes seguintes (#43 a #49).
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
#include <secrets.h>
#include <storage.h>

#include "portal.h"

const byte PORTA_DNS = 53;

DNSServer dns;

// Global, fora da pilha do loop(): dez contas ocupam uns 2,5 KB.
StaticJsonDocument<3072> professores;

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("Presente! - firmware, pacote 1 (#41)");

  feedback::iniciar();
  feedback::acender(feedback::Cor::AZUL);  // AGUARDANDO_LOGIN

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
  WiFi.mode(WIFI_AP_STA);  // o STA fica parado ate a busca da turma (pacote 2)
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
  portal::atender();
  feedback::atualizar();
}
