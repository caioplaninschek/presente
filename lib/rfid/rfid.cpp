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

// Memoria da janela de silencio (R30).
Uid ultimoUid = {{0}, 0};
unsigned long instanteDaUltimaLeitura = 0;
bool houveLeituraAceita = false;

// micros() do comeco da tentativa em curso, guardado antes de falar com o
// RC522: e dele que sai o numero que o alvo de 200 ms da secao 5 cobra.
unsigned long inicioDaTentativaUs = 0;

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

bool mesmoUid(const Uid& a, const Uid& b) {
  if (a.tamanho != b.tamanho || a.tamanho == 0) {
    return false;
  }
  for (byte i = 0; i < a.tamanho; i++) {
    if (a.bytes[i] != b.bytes[i]) {
      return false;
    }
  }
  return true;
}

unsigned long inicioDaLeituraUs() {
  return inicioDaTentativaUs;
}

void esquecerUltimaTag() {
  houveLeituraAceita = false;
  ultimoUid.tamanho = 0;
}

bool lerTagNova(Uid& uid) {
  // Marcado antes de qualquer conversa com o RC522: o tempo do barramento SPI
  // esta dentro do intervalo que a secao 5 limita a 200 ms, e e ele que pesa.
  const unsigned long comecouEm = micros();

  if (!leitor.PICC_IsNewCardPresent()) {
    return false;
  }
  if (!leitor.PICC_ReadCardSerial()) {
    return false;
  }

  Uid lido;
  lido.tamanho = leitor.uid.size;
  if (lido.tamanho > UID_MAX_BYTES) {
    lido.tamanho = UID_MAX_BYTES;
  }
  for (byte i = 0; i < lido.tamanho; i++) {
    lido.bytes[i] = leitor.uid.uidByte[i];
  }

  // Manda a tag dormir: enquanto ela ficar no campo, nao responde de novo. E o
  // que faz o cracha esquecido em cima do leitor nao virar uma enxurrada de
  // leituras. Nao ha PCD_StopCrypto1 porque nunca houve autenticacao: o UID sai
  // na resposta do anticolisao, sem ler bloco nenhum e sem escrever (HU-22).
  leitor.PICC_HaltA();

  // Janela de silencio (R30): o mesmo UID de novo, cedo demais, e descartado
  // aqui dentro. Quem chamou nao fica sabendo, e e essa a intencao -- nenhum
  // feedback acontece, nem verde nem bip. A janela conta da ultima leitura
  // aceita, entao encostar sem parar nao a empurra para frente.
  if (houveLeituraAceita && mesmoUid(lido, ultimoUid) &&
      millis() - instanteDaUltimaLeitura < JANELA_DE_SILENCIO_MS) {
    return false;
  }

  ultimoUid = lido;
  instanteDaUltimaLeitura = millis();
  houveLeituraAceita = true;
  inicioDaTentativaUs = comecouEm;

  uid = lido;
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
