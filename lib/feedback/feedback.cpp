#include "feedback.h"

namespace feedback {
namespace {

// Pinos da secao 4.1 da spec. Cada perna do LED tem 220 ohm em serie.
const byte PINO_VERMELHO = 25;
const byte PINO_VERDE = 26;
const byte PINO_AZUL = 27;
const byte PINO_BUZZER = 33;

// O ponto unico de inversao da polaridade do LED. O porque e o que fazer com
// ele estao no cabecalho do feedback.h. true = catodo comum (perna comum no
// GND, nivel alto acende), que e o que a secao 4.1 especifica.
const bool CATODO_COMUM = true;

Cor cor = Cor::APAGADO;
bool buzzerLigado = false;
unsigned long inicioDoBip = 0;
unsigned long duracaoDoBip = 0;

// Toda escrita no LED passa por aqui: e o que faz a constante acima bastar.
void escreverNaPerna(byte pino, bool acesa) {
  const bool nivel = CATODO_COMUM ? acesa : !acesa;
  digitalWrite(pino, nivel ? HIGH : LOW);
}

}  // namespace

void iniciar() {
  pinMode(PINO_VERMELHO, OUTPUT);
  pinMode(PINO_VERDE, OUTPUT);
  pinMode(PINO_AZUL, OUTPUT);
  pinMode(PINO_BUZZER, OUTPUT);

  acender(Cor::APAGADO);
  digitalWrite(PINO_BUZZER, LOW);
  buzzerLigado = false;
}

void acender(Cor novaCor) {
  cor = novaCor;
  escreverNaPerna(PINO_VERMELHO, novaCor == Cor::VERMELHO);
  escreverNaPerna(PINO_VERDE, novaCor == Cor::VERDE);
  escreverNaPerna(PINO_AZUL, novaCor == Cor::AZUL);
}

Cor corAtual() {
  return cor;
}

const char* nomeDaCor(Cor umaCor) {
  switch (umaCor) {
    case Cor::VERMELHO: return "vermelho";
    case Cor::VERDE:    return "verde";
    case Cor::AZUL:     return "azul";
    default:            return "apagado";
  }
}

void bipar(unsigned long duracaoMs) {
  if (duracaoMs == 0) {
    return;
  }
  inicioDoBip = millis();
  duracaoDoBip = duracaoMs;
  buzzerLigado = true;
  digitalWrite(PINO_BUZZER, HIGH);  // buzzer ativo: nivel alto ja e som
}

bool bipando() {
  return buzzerLigado;
}

void atualizar() {
  if (!buzzerLigado) {
    return;
  }
  if (millis() - inicioDoBip >= duracaoDoBip) {
    digitalWrite(PINO_BUZZER, LOW);
    buzzerLigado = false;
  }
}

}  // namespace feedback
