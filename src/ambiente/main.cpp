// ambiente/main.cpp — programa da issue #14.
//
// Prova que o computador compila o projeto e grava na placa: pisca o LED
// da placa e escreve uma frase por segundo no monitor serial.
// O tempo e contado com millis(), sem travar o loop (regra da secao 5 da spec).

#include <Arduino.h>

// LED azul do DevKit V1 de 30 pinos. Outra placa pode nao ter LED neste pino;
// nesse caso a frase no monitor serial continua valendo como prova.
const int LED_PLACA = 2;

unsigned long ultimoMarco = 0;
unsigned long segundos = 0;

void setup() {
  pinMode(LED_PLACA, OUTPUT);
  Serial.begin(115200);
  ultimoMarco = millis();
}

void loop() {
  if (millis() - ultimoMarco >= 1000) {
    ultimoMarco += 1000;  // soma o marco em vez de ler millis() de novo: sem deriva
    segundos++;
    digitalWrite(LED_PLACA, !digitalRead(LED_PLACA));
    Serial.print("Presente! Ambiente funcionando - ");
    Serial.print(segundos);
    Serial.println(" s");
  }
}
