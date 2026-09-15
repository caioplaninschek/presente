// prova-de-vida/main.cpp -- programa da issue #24.
//
// O primeiro programa que mexe nas pecas. Em ciclo: vermelho por um segundo,
// verde por um segundo, azul por um segundo, apaga e toca o bip de 100 ms. O
// nome da cor que esta acesa sai no monitor serial -- e o criterio de pronto da
// issue e justamente o nome bater com o que o olho ve.
//
// Nao ha cracha, nao ha estado e nao ha maquina de estados: isso e a #26. Este
// programa so prova que o LED e o buzzer obedecem a comando.
//
// O mecanismo mora em lib/feedback/, porque a #26 usa o mesmo (R28). Aqui esta
// so o roteiro da demonstracao.
//
// LED que nao acende nenhuma cor: a peca e de anodo comum, e a inversao e uma
// linha so -- a constante CATODO_COMUM em lib/feedback/feedback.cpp. O cabecalho
// do lib/feedback/feedback.h explica.
//
// Loop sem delay(), tudo por millis(), como manda a secao 5 da spec. Aqui isso
// nao muda o resultado; muda o habito, e e o habito que sustenta o aparelho
// atender o portal e ler cracha ao mesmo tempo mais adiante.

#include <Arduino.h>
#include <feedback.h>

// Cada passo do ciclo dura o mesmo tanto.
const unsigned long DURACAO_DO_PASSO_MS = 1000;

// A duracao que a maquina de estados da secao 5 da spec da ao bip de
// REGISTRADO. Aqui serve para ouvir a peca; la vai significar "voce foi
// registrado".
const unsigned long DURACAO_DO_BIP_MS = 100;

const feedback::Cor CICLO[] = {
  feedback::Cor::VERMELHO,
  feedback::Cor::VERDE,
  feedback::Cor::AZUL,
  feedback::Cor::APAGADO,
};
const byte PASSOS_DO_CICLO = sizeof(CICLO) / sizeof(CICLO[0]);

byte passo = 0;
unsigned long ultimoMarco = 0;

void executarPasso() {
  const feedback::Cor cor = CICLO[passo];
  feedback::acender(cor);

  Serial.print(feedback::nomeDaCor(cor));

  // O bip acompanha o passo em que o LED apaga, para nao disputar a atencao de
  // quem esta conferindo a cor.
  if (cor == feedback::Cor::APAGADO) {
    feedback::bipar(DURACAO_DO_BIP_MS);
    Serial.print(" + bip de ");
    Serial.print(DURACAO_DO_BIP_MS);
    Serial.print(" ms");
  }
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  feedback::iniciar();

  Serial.println();
  Serial.println("Presente! - prova de vida do LED e do buzzer (issue #24)");
  Serial.println("Cada cor dura 1 s. O bip toca quando o LED apaga.");

  ultimoMarco = millis();
  executarPasso();
}

void loop() {
  // Desliga o buzzer quando os 100 ms acabarem. Sem esta linha o bip nao para.
  feedback::atualizar();

  if (millis() - ultimoMarco >= DURACAO_DO_PASSO_MS) {
    ultimoMarco += DURACAO_DO_PASSO_MS;  // soma o marco: sem deriva
    passo = (passo + 1) % PASSOS_DO_CICLO;
    executarPasso();
  }
}
