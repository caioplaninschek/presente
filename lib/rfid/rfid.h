// lib/rfid/rfid.h -- leitura de UID no RC522.
//
// Mora em lib/ porque mais de um programa de bancada usa: a #25 imprime o UID
// no monitor serial e a #26 acende o LED e toca o bip em cima da mesma leitura
// (R28: codigo compartilhado sobe para lib/<modulo da secao 5>/).
//
// Este modulo e o embriao do rfid.cpp da secao 5 da spec. A janela de silencio
// de 5 s (R30) ainda NAO mora aqui: ela entra com a #26, que e quem tem
// feedback para segurar. Aqui, uma tag lida e uma tag que entrou no campo.
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

// true uma vez por tag que entra no campo, preenchendo uid. O cracha parado no
// leitor nao repete: depois de ler, a tag e mandada dormir (HaltA) e so volta a
// responder quando sair do campo e encostar de novo.
bool lerTagNova(Uid& uid);

// Escreve o UID em hexadecimal maiusculo e sem separador -- "A391F21B" -- que e
// o formato do campo uid no contrato de dados (secao 7 da spec). destino precisa
// de UID_TEXTO_TAMANHO bytes.
void formatarUid(const Uid& uid, char* destino, size_t tamanhoDoDestino);

}  // namespace rfid

#endif  // RFID_H
