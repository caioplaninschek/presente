// persistencia/main.cpp -- programa da issue #27.
//
// O aparelho fica preso na parede de uma sala, ligado na tomada. Uma queda de
// energia no meio da aula nao pode apagar as presencas ja registradas, senao o
// professor refaz a chamada inteira (HU-35). Cobre o teste 8 da secao 8 da spec.
//
// E a #26 com o eventos.json atras dela. O feedback e o mesmo, a regra e a
// mesma; o que muda e que a lista de quem ja esta registrado sobrevive ao
// reinicio, porque o arquivo e relido no boot (R30).
//
// ---------------------------------------------------------------------------
// O QUE SE GRAVA, E POR QUE NAO E O OBJETO DO SECAO 7
//
// Uma linha por evento, cada linha um objeto JSON completo terminado em \n,
// escrita so por append:
//
//   {"uid":"A391F21B","ms_desde_boot":12345,"origem":"nfc"}
//
// O arquivo inteiro nao e um JSON valido, e e de proposito. O array do secao 7
// se monta na saida -- no Exibir, no Compartilhar e no Enviar. Manter o array
// fechado em disco obrigaria a reescrever o fecho a cada evento, e a queda de
// energia no meio dessa reescrita levaria o arquivo todo, nao um registro.
//
// ms_desde_boot nao e timestamp. O timestamp do secao 7 e ISO-8601 com fuso e
// vem do navegador do professor no Iniciar, que ainda nao existe. O porque
// inteiro esta no cabecalho do lib/storage/storage.h.
// ---------------------------------------------------------------------------
//
// Comandos no monitor serial, para o passo 4 do roteiro de bancada:
//   l  imprime o conteudo do arquivo
//   z  apaga o arquivo e comeca o teste do zero
//
// Loop sem delay(), como sempre (secao 5 da spec).

#include <Arduino.h>
#include <feedback.h>
#include <rfid.h>
#include <storage.h>

const unsigned long BIP_MS = 100;
const unsigned long PISCADA_ACESA_MS = 90;
const unsigned long PISCADA_APAGADA_MS = 90;
const unsigned long INTERVALO_DO_AVISO_MS = 3000;

bool leitorVivo = false;
bool arquivoVivo = false;
unsigned long ultimoAviso = 0;

void avisarLeitorMudo() {
  const byte versao = rfid::versaoDoLeitor();
  Serial.print("RC522 nao respondeu - VersionReg = 0x");
  if (versao < 0x10) {
    Serial.print("0");
  }
  Serial.println(versao, HEX);
  Serial.println("  Confira 3,3 V no RC522, o RST no pino 22 e o SDA no pino 5.");
}

void listarArquivo() {
  Serial.println("--- eventos.json ---");
  const size_t linhas = storage::imprimirEventos(Serial);
  Serial.print("--- ");
  Serial.print(linhas);
  Serial.print(" linha(s); ");
  Serial.print(storage::quantidade());
  Serial.println(" cracha(s) diferente(s) registrado(s) ---");
}

void atenderComando() {
  if (!Serial.available()) {
    return;
  }
  const char comando = Serial.read();

  if (comando == 'l' || comando == 'L') {
    listarArquivo();
    return;
  }

  if (comando == 'z' || comando == 'Z') {
    if (storage::apagarEventos()) {
      Serial.println("eventos.json apagado. A sessao recomeca vazia.");
    } else {
      Serial.println("Nao consegui apagar o eventos.json.");
    }
  }
}

void setup() {
  Serial.begin(115200);
  rfid::iniciar();
  feedback::iniciar();

  Serial.println();
  Serial.println("Presente! - a presenca nao se perde (issue #27)");

  const int reencontrados = storage::iniciarSessaoComArquivo();
  arquivoVivo = reencontrados >= 0;

  if (!arquivoVivo) {
    Serial.println("LittleFS nao montou: nada sera gravado, e o teste 8 nao vale.");
  } else {
    Serial.print("Relido do eventos.json: ");
    Serial.print(reencontrados);
    Serial.print(" evento(s), ");
    Serial.print(storage::quantidade());
    Serial.println(" cracha(s) ja registrado(s).");
    // Depois do tranco de energia, este numero e o que diz se a presenca se
    // perdeu: ele tem que continuar de onde parou, e nao voltar a zero.
  }

  ultimoAviso = millis();
  leitorVivo = rfid::leitorRespondeu();
  if (!leitorVivo) {
    avisarLeitorMudo();
    return;
  }

  Serial.println("Encoste uma tag. 'l' imprime o arquivo, 'z' apaga tudo.");
}

void loop() {
  feedback::atualizar();
  atenderComando();

  if (!leitorVivo) {
    if (millis() - ultimoAviso >= INTERVALO_DO_AVISO_MS) {
      ultimoAviso += INTERVALO_DO_AVISO_MS;
      avisarLeitorMudo();
    }
    return;
  }

  rfid::Uid uid;
  if (!rfid::lerTagNova(uid)) {
    return;
  }

  const bool jaEstava = storage::jaRegistrado(uid);
  const bool gravou = storage::registrar(uid, millis());

  if (gravou) {
    feedback::piscar(feedback::Cor::VERDE, 2, PISCADA_ACESA_MS, PISCADA_APAGADA_MS);
    feedback::bipar(BIP_MS);
  } else {
    // Quem ja estava registrado recebe a piscada unica da #26 (R30). Falha de
    // gravacao tambem cai aqui, e por isso os dois casos se distinguem no log:
    // um e comportamento esperado, o outro e defeito.
    feedback::piscar(feedback::Cor::VERDE, 1, PISCADA_ACESA_MS, PISCADA_APAGADA_MS);
  }

  char uidTexto[rfid::UID_TEXTO_TAMANHO];
  rfid::formatarUid(uid, uidTexto, sizeof(uidTexto));

  Serial.print(uidTexto);
  Serial.print("  ");
  Serial.print(uid.tamanho);
  Serial.print(" bytes  ");

  if (gravou) {
    Serial.print("gravado");
  } else if (jaEstava) {
    Serial.print("ja registrado - nada gravado");
  } else if (storage::cheia()) {
    Serial.print("FALHOU: a lista encheu");
  } else {
    Serial.print("FALHOU: nao consegui gravar no arquivo");
  }

  Serial.print("  (");
  Serial.print(storage::quantidade());
  Serial.println(" no total)");
}
