// lib/rfid/rfid.h -- leitura de UID no RC522.
//
// Mora em lib/ porque mais de um programa de bancada usa: a #25 imprime o UID
// no monitor serial e a #26 acende o LED e toca o bip em cima da mesma leitura
// (R28: codigo compartilhado sobe para lib/<modulo da secao 5>/).
//
// Este modulo e o embriao do rfid.cpp da secao 5 da spec, que lhe atribui a
// leitura de UID de 4 e 7 bytes e a janela de silencio de 5 s (R30). A janela
// entrou com a #26, que foi quem trouxe feedback para ela segurar.
//
// So leitura: nada nunca e escrito na tag (HU-22).

#ifndef RFID_H
#define RFID_H

#include <Arduino.h>

namespace rfid {

// 10 bytes cobre os tres tamanhos que o padrao preve (4, 7 e 10). O projeto
// so usa 4 (Mifare Classic) e 7 (NTAG215), mas o tamanho nao e assumido em
// lugar nenhum do codigo -- e sempre o que a tag respondeu.
const byte UID_MAX_BYTES = 10;

// Dois caracteres por byte, mais o terminador.
const size_t UID_TEXTO_TAMANHO = UID_MAX_BYTES * 2 + 1;

struct Uid {
  byte bytes[UID_MAX_BYTES];
  byte tamanho;  // em bytes: 4, 7 ou 10
};

// Liga o SPI e o RC522 nos pinos da secao 4.1 da spec.
void iniciar();

// false quando o RC522 nao respondeu ao SPI. Le o VersionReg: 0x00 e 0xFF nao
// sao versoes, sao o barramento em silencio -- fio solto ou alimentacao errada.
bool leitorRespondeu();

// O byte cru do VersionReg, para o log dizer o que foi lido de verdade.
byte versaoDoLeitor();

// Quanto tempo o mesmo UID fica em silencio depois de uma leitura aceita. Valor
// de partida, nao valor sagrado: calibrar faz parte do teste (secao 8 da spec).
const unsigned long JANELA_DE_SILENCIO_MS = 5000;

// true uma vez por tag que entra no campo, preenchendo uid.
//
// Duas coisas diferentes seguram a repeticao, e as duas sao queridas:
//   - depois de ler, a tag e mandada dormir (HaltA), entao o cracha esquecido
//     em cima do leitor nao produz leitura nenhuma enquanto ficar la;
//   - o mesmo UID lido de novo dentro de JANELA_DE_SILENCIO_MS e descartado em
//     silencio, sem feedback nenhum (R30), que e o caso do aluno que encosta,
//     tira e encosta de novo.
//
// A janela conta a partir da ultima leitura ACEITA, nao da ultima tentativa:
// encostar sem parar nao empurra a janela para frente indefinidamente.
bool lerTagNova(Uid& uid);

// micros() do instante em que comecou a tentativa de leitura que deu certo --
// antes da conversa com o RC522, nao depois dela. E o comeco do intervalo que a
// secao 5 da spec limita a 200 ms ("do encostar da tag ate o feedback"). So faz
// sentido logo depois de lerTagNova() ter devolvido true.
unsigned long inicioDaLeituraUs();

// Descarta a memoria da janela de silencio: a proxima leitura vale, mesmo sendo
// do mesmo UID.
void esquecerUltimaTag();

// true quando os dois UIDs tem o mesmo tamanho e os mesmos bytes.
bool mesmoUid(const Uid& a, const Uid& b);

// Escreve o UID em hexadecimal maiusculo e sem separador -- "A391F21B" -- que e
// o formato do campo uid no contrato de dados (secao 7 da spec). destino precisa
// de UID_TEXTO_TAMANHO bytes.
void formatarUid(const Uid& uid, char* destino, size_t tamanhoDoDestino);

// A volta do formatarUid: le "A391F21B" e devolve o UID. false se o texto nao
// for um numero par de digitos hexadecimais dentro do tamanho maximo. E o que
// permite ao eventos.json ser relido no boot (R30).
bool uidDeTexto(const char* texto, Uid& uid);

}  // namespace rfid

#endif  // RFID_H
