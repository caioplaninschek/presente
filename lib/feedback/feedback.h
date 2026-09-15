// lib/feedback/feedback.h -- LED RGB e buzzer.
//
// Mora em lib/ porque mais de um programa de bancada usa: a #24 percorre as
// tres cores para provar que as pecas obedecem e a #26 pisca o verde e toca o
// bip quando o cracha e lido (R28: codigo compartilhado sobe para
// lib/<modulo da secao 5>/).
//
// Este modulo e o embriao do feedback.cpp da secao 5 da spec, mas ainda nao e
// ele: aqui estao o mecanismo e os tempos, nao a maquina de estados. REGISTRADO,
// JA_REGISTRADO e os demais estados entram com a #26, que e quem tem leitura de
// cracha para reagir. Aqui ninguem sabe por que o verde acendeu.
//
// ---------------------------------------------------------------------------
// POLARIDADE DO LED -- o ponto unico de inversao
//
// O programa e escrito para LED de CATODO COMUM, como manda a secao 4.1 da spec:
// a perna comum vai ao GND e o nivel alto acende. Se a peca da bancada for de
// ANODO COMUM (perna comum no 3,3 V), nenhuma cor acende, e a correcao e trocar
// UMA linha:
//
//     em lib/feedback/feedback.cpp, a constante CATODO_COMUM passa de true para
//     false.
//
// Nao ha outro lugar a mexer: os pinos tambem moram la, e todo digitalWrite do
// LED passa por essa constante. Nao inverta o valor dentro de acender().
// ---------------------------------------------------------------------------
//
// O buzzer e ativo (secao 4.1): nivel alto ja produz o som, sem tone() e sem PWM.

#ifndef FEEDBACK_H
#define FEEDBACK_H

#include <Arduino.h>

namespace feedback {

enum class Cor { APAGADO, VERMELHO, VERDE, AZUL };

// Liga os quatro pinos como saida e deixa tudo apagado e em silencio.
void iniciar();

// Acende uma cor por vez. APAGADO desliga as tres.
void acender(Cor cor);

Cor corAtual();

// "vermelho", "verde", "azul" ou "apagado" -- para o log dizer a mesma coisa
// que o olho ve, que e o criterio de pronto da #24.
const char* nomeDaCor(Cor cor);

// Comeca um bip de duracaoMs e volta na hora. Quem desliga o buzzer e o
// atualizar(), porque o loop() do projeto nao usa delay() (secao 5 da spec).
// Chamar de novo durante um bip estende o bip em curso a partir de agora.
void bipar(unsigned long duracaoMs);

bool bipando();

// Precisa ser chamada a cada volta do loop(). Sem ela o buzzer que comecou a
// tocar nao para.
void atualizar();

}  // namespace feedback

#endif  // FEEDBACK_H
