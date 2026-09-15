#include "rfid.h"

#include <MFRC522.h>
#include <SPI.h>

namespace rfid {
namespace {

// Pinos da secao 4.1 da spec. SCK 18, MOSI 23 e MISO 19 sao o VSPI por
// hardware e ficam implicitos no SPI.begin() padrao do ESP32.
const byte PINO_SS = 5;    // pino de strapping: precisa estar alto no boot
const byte PINO_RST = 22;

MFRC522 leitor(PINO_SS, PINO_RST);

}  // namespace

void iniciar() {
  SPI.begin();
  leitor.PCD_Init();
}

byte versaoDoLeitor() {
  return leitor.PCD_ReadRegister(MFRC522::VersionReg);
}

bool leitorRespondeu() {
  const byte versao = versaoDoLeitor();
  // 0x00 e 0xFF sao as duas leituras que o SPI devolve quando ninguem responde:
  // linha presa no GND e linha presa no VCC. Qualquer outro valor e um chip
  // falando -- 0x91 e 0x92 sao as versoes de fabrica, e os clones respondem
  // outros numeros que funcionam igual.
  return versao != 0x00 && versao != 0xFF;
}

bool lerTagNova(Uid& uid) {
  if (!leitor.PICC_IsNewCardPresent()) {
    return false;
  }
  if (!leitor.PICC_ReadCardSerial()) {
    return false;
  }

  uid.tamanho = leitor.uid.size;
  if (uid.tamanho > UID_MAX_BYTES) {
    uid.tamanho = UID_MAX_BYTES;
  }
  for (byte i = 0; i < uid.tamanho; i++) {
    uid.bytes[i] = leitor.uid.uidByte[i];
  }

  // Manda a tag dormir: enquanto ela ficar no campo, nao responde de novo. E o
  // que faz o cracha esquecido em cima do leitor nao virar uma enxurrada de
  // leituras. Nao ha PCD_StopCrypto1 porque nunca houve autenticacao: o UID sai
  // na resposta do anticolisao, sem ler bloco nenhum e sem escrever (HU-22).
  leitor.PICC_HaltA();
  return true;
}

void formatarUid(const Uid& uid, char* destino, size_t tamanhoDoDestino) {
  if (destino == nullptr || tamanhoDoDestino == 0) {
    return;
  }

  static const char DIGITOS[] = "0123456789ABCDEF";
  size_t escritos = 0;
  for (byte i = 0; i < uid.tamanho && escritos + 2 < tamanhoDoDestino; i++) {
    destino[escritos++] = DIGITOS[uid.bytes[i] >> 4];
    destino[escritos++] = DIGITOS[uid.bytes[i] & 0x0F];
  }
  destino[escritos] = '\0';
}

}  // namespace rfid
