#include "feedback.h"

namespace feedback {
namespace {

// Pinos da secao 4.1 da spec. Cada perna do LED tem um resistor em serie: a
// montagem de 21/09 usa 300 ohm, onde a spec previa 220 (R44).
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

// Piscada em curso.
bool haPiscada = false;
Cor corDaPiscada = Cor::APAGADO;
Cor corDeFundo = Cor::APAGADO;  // o que estava aceso antes, e para onde se volta
byte piscadasQueFaltam = 0;
bool piscadaAcesa = false;
unsigned long inicioDaFase = 0;
unsigned long duracaoAceso = 0;
unsigned long duracaoApagado = 0;

// Toda escrita no LED passa por aqui: e o que faz a constante acima bastar.
void escreverNaPerna(byte pino, bool acesa) {
  const bool nivel = CATODO_COMUM ? acesa : !acesa;
  digitalWrite(pino, nivel ? HIGH : LOW);
}

// Escreve no LED sem mexer na piscada em curso nem na cor de fundo.
void mostrar(Cor umaCor) {
  cor = umaCor;
  escreverNaPerna(PINO_VERMELHO, umaCor == Cor::VERMELHO);
  escreverNaPerna(PINO_VERDE, umaCor == Cor::VERDE);
  escreverNaPerna(PINO_AZUL, umaCor == Cor::AZUL);
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
  haPiscada = false;
  corDeFundo = novaCor;
  mostrar(novaCor);
}

void piscar(Cor novaCor, byte vezes, unsigned long acesoMs, unsigned long apagadoMs) {
  if (vezes == 0) {
    return;
  }
  // A cor de fundo so se guarda se nao houver piscada em curso; senao uma
  // piscada que interrompe outra passaria a "voltar" para a cor da anterior.
  if (!haPiscada) {
    corDeFundo = cor;
  }
  haPiscada = true;
  corDaPiscada = novaCor;
  piscadasQueFaltam = vezes;
  duracaoAceso = acesoMs;
  duracaoApagado = apagadoMs;
  piscadaAcesa = true;
  inicioDaFase = millis();
  mostrar(novaCor);
}

bool piscando() {
  return haPiscada;
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
  digitalWrite(PINO_BUZZER, HIGH);  // buzzer ativo: nivel alto ja e som.
  // Buzzer passivo so estala com nivel constante; e a hipotese da R44 para o
  // som fraco de 21/09, a conferir na peca antes de mexer aqui.
}

bool bipando() {
  return buzzerLigado;
}

void atualizar() {
  const unsigned long agora = millis();

  if (buzzerLigado && agora - inicioDoBip >= duracaoDoBip) {
    digitalWrite(PINO_BUZZER, LOW);
    buzzerLigado = false;
  }

  if (!haPiscada) {
    return;
  }

  if (piscadaAcesa) {
    if (agora - inicioDaFase >= duracaoAceso) {
      piscadaAcesa = false;
      inicioDaFase += duracaoAceso;
      mostrar(Cor::APAGADO);
      piscadasQueFaltam--;
      if (piscadasQueFaltam == 0) {
        // Terminou na fase apagada; o intervalo seguinte fecha a piscada e
        // devolve o LED ao que estava aceso antes dela.
        if (duracaoApagado == 0) {
          haPiscada = false;
          mostrar(corDeFundo);
        }
      }
    }
    return;
  }

  if (agora - inicioDaFase >= duracaoApagado) {
    inicioDaFase += duracaoApagado;
    if (piscadasQueFaltam == 0) {
      haPiscada = false;
      mostrar(corDeFundo);
    } else {
      piscadaAcesa = true;
      mostrar(corDaPiscada);
    }
  }
}

}  // namespace feedback
