// lib/config/config.h -- os valores fixos do aparelho, num lugar so (secao 5
// da spec, R66).
//
// Mora em lib/ e nao em include/ porque as bibliotecas de lib/ nao enxergam o
// include/ do projeto, e um include/config.h ainda perderia para o config.h do
// proprio framework. Em lib/config/, as bibliotecas e os programas de src/
// acham este arquivo com #include <config.h>.
//
// O nome e a senha da rede ficam no secrets.h (secao 5). Os tempos entram no
// pacote que os usar.

#pragma once

#include <stdint.h>

// --- Pinos da secao 4.1 da spec ----------------------------------

// RC522. SCK 18, MOSI 23 e MISO 19 sao o VSPI por hardware e ficam implicitos
// no SPI.begin() padrao do ESP32.
constexpr uint8_t PINO_SS = 5;    // pino de strapping: precisa estar alto no boot
constexpr uint8_t PINO_RST = 22;

// LED RGB e buzzer. Cada perna do LED tem um resistor em serie: a montagem de
// 21/09 usa 300 ohm, onde a spec previa 220 (R44). A polaridade do LED nao e
// pino: mora no lib/feedback/feedback.cpp (CATODO_COMUM).
constexpr uint8_t PINO_VERMELHO = 25;
constexpr uint8_t PINO_VERDE = 26;
constexpr uint8_t PINO_AZUL = 27;
constexpr uint8_t PINO_BUZZER = 33;

// --- Turma e disciplina (R48) -------------------------------------
// Fixas no aparelho; o portal so as exibe.
constexpr char TURMA[] = "4172CMPN6A_P1";
constexpr char DISCIPLINA[] = "Sistemas Embarcados";

// --- Endereco do aparelho na propria rede -------------------------
// Para onde o portal cativo manda o celular.
constexpr char ENDERECO_DO_APARELHO[] = "192.168.4.1";
