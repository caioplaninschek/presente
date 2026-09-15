// leitor/main.cpp -- programa da issue #25.
//
// Encosta a tag, o numero dela aparece no monitor serial. Nada mais: sem LED,
// sem buzzer e sem gravar -- isso e a #26 e a #27.
//
// O UID sai em hexadecimal maiusculo e sem separador (A391F21B), que e o
// formato do campo uid no contrato de dados da secao 7 da spec. O que aparece
// aqui e o que vai virar dado de teste no banco, na #21, entao os dois tem que
// ser a mesma string.
//
// A leitura em si mora em lib/rfid/, porque a #26 usa a mesma (R28).
// Loop sem delay(), tudo por millis(), como manda a secao 5 da spec.

#include <Arduino.h>
#include <rfid.h>

// De quanto em quanto tempo o aviso de leitor mudo se repete. O monitor serial
// costuma abrir depois do boot e comeria a linha se ela fosse impressa uma vez
// so -- e essa e justamente a linha que diz por que nada acontece.
const unsigned long INTERVALO_DO_AVISO_MS = 3000;

bool leitorVivo = false;
unsigned long ultimoAviso = 0;

void imprimirDiagnostico() {
  const byte versao = rfid::versaoDoLeitor();

  Serial.print("RC522 nao respondeu - VersionReg = 0x");
  if (versao < 0x10) {
    Serial.print("0");
  }
  Serial.println(versao, HEX);
  Serial.println("  0x00 ou 0xFF significa barramento em silencio, nao versao.");
  Serial.println("  Confira, nesta ordem: 3,3 V no RC522 (5 V queima o modulo),");
  Serial.println("  o fio do RST no pino 22 e o do SDA no pino 5.");
}

void setup() {
  Serial.begin(115200);
  rfid::iniciar();

  Serial.println();
  Serial.println("Presente! - leitor de cracha (issue #25)");

  leitorVivo = rfid::leitorRespondeu();
  if (leitorVivo) {
    const byte versao = rfid::versaoDoLeitor();
    Serial.print("RC522 respondeu - VersionReg = 0x");
    Serial.println(versao, HEX);
    Serial.println("Encoste uma tag.");
  } else {
    imprimirDiagnostico();
  }

  ultimoAviso = millis();
}

void loop() {
  // Leitor mudo: nao adianta procurar tag. Repete o diagnostico e espera que
  // alguem mexa na fiacao e aperte o reset.
  if (!leitorVivo) {
    if (millis() - ultimoAviso >= INTERVALO_DO_AVISO_MS) {
      ultimoAviso += INTERVALO_DO_AVISO_MS;
      imprimirDiagnostico();
    }
    return;
  }

  rfid::Uid uid;
  if (!rfid::lerTagNova(uid)) {
    return;
  }

  char texto[rfid::UID_TEXTO_TAMANHO];
  rfid::formatarUid(uid, texto, sizeof(texto));

  // Uma linha por tag, o numero primeiro: e essa coluna que o Joao copia para
  // o comentario da issue. O tamanho vem junto porque a #25 pede tag de 4 e de
  // 7 bytes, e e ele que diz qual das duas acabou de ser lida.
  Serial.print(texto);
  Serial.print("  (");
  Serial.print(uid.tamanho);
  Serial.println(" bytes)");
}
