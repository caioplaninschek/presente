#include "storage.h"

#include <ArduinoJson.h>
#include <LittleFS.h>

namespace storage {
namespace {

rfid::Uid registrados[CAPACIDADE];
byte total = 0;
bool comArquivo = false;

// Uma linha cabe folgada aqui: o UID mais longo tem 20 digitos e os outros dois
// campos sao curtos. Buffer fixo, sem String, pela regra de performance do
// secao 5 da spec.
const size_t LINHA_MAX = 128;

// Documento estatico, nunca dinamico (R29): uma linha por vez, sempre do mesmo
// tamanho, e o heap nao fragmenta depois de dezenas de eventos.
const size_t DOC_TAMANHO = 128;

bool adicionarNaMemoria(const rfid::Uid& uid) {
  if (total >= CAPACIDADE) {
    return false;
  }
  registrados[total] = uid;
  total++;
  return true;
}

// Le uma linha ate o \n. Devolve false no fim do arquivo. completa diz se a
// linha terminou em \n de verdade: a que nao terminou e o registro que a queda
// de energia pegou no meio da escrita, e nao vale.
bool lerLinha(File& arquivo, char* destino, size_t tamanho, bool& completa) {
  size_t escritos = 0;
  completa = false;

  if (!arquivo.available()) {
    return false;
  }

  while (arquivo.available()) {
    const int c = arquivo.read();
    if (c < 0) {
      break;
    }
    if (c == '\n') {
      completa = true;
      break;
    }
    if (c == '\r') {
      continue;
    }
    if (escritos + 1 < tamanho) {
      destino[escritos++] = static_cast<char>(c);
    }
  }

  destino[escritos] = '\0';
  return true;
}

}  // namespace

void iniciarSessaoNaMemoria() {
  total = 0;
  comArquivo = false;
}

bool persistindo() {
  return comArquivo;
}

int iniciarSessaoComArquivo() {
  total = 0;
  comArquivo = false;

  // true formata se a montagem falhar. Numa placa nova nao ha sistema de
  // arquivos nenhum, e sem isso o primeiro boot nao teria onde gravar. Se a
  // montagem falhou, o que estivesse la ja estava ilegivel.
  if (!LittleFS.begin(true)) {
    return -1;
  }
  comArquivo = true;

  if (!LittleFS.exists(CAMINHO_DOS_EVENTOS)) {
    return 0;
  }

  File arquivo = LittleFS.open(CAMINHO_DOS_EVENTOS, "r");
  if (!arquivo) {
    // O arquivo existe e nao abre: nao da para saber quem ja esta registrado, e
    // gravar por cima seria pior que nao gravar.
    comArquivo = false;
    return -1;
  }

  int lidos = 0;
  char linha[LINHA_MAX];
  bool completa = false;

  while (lerLinha(arquivo, linha, sizeof(linha), completa)) {
    if (!completa || linha[0] == '\0') {
      continue;  // linha truncada pela queda de energia, ou linha vazia
    }

    StaticJsonDocument<DOC_TAMANHO> doc;
    if (deserializeJson(doc, linha) != DeserializationError::Ok) {
      continue;
    }

    const char* uidTexto = doc["uid"];
    rfid::Uid uid;
    if (uidTexto == nullptr || !rfid::uidDeTexto(uidTexto, uid)) {
      continue;
    }

    lidos++;
    if (!jaRegistrado(uid)) {
      adicionarNaMemoria(uid);
    }
  }

  arquivo.close();
  return lidos;
}

bool jaRegistrado(const rfid::Uid& uid) {
  for (byte i = 0; i < total; i++) {
    if (rfid::mesmoUid(registrados[i], uid)) {
      return true;
    }
  }
  return false;
}

bool registrar(const rfid::Uid& uid, unsigned long msDesdeBoot) {
  if (jaRegistrado(uid)) {
    return false;
  }
  if (cheia()) {
    return false;
  }

  if (comArquivo) {
    char uidTexto[rfid::UID_TEXTO_TAMANHO];
    rfid::formatarUid(uid, uidTexto, sizeof(uidTexto));

    StaticJsonDocument<DOC_TAMANHO> doc;
    doc["uid"] = uidTexto;
    doc["ms_desde_boot"] = msDesdeBoot;
    doc["origem"] = "nfc";

    // "a" e append: o cursor nasce no fim e nada do que ja esta escrito e
    // tocado. E o que faz uma queda de energia custar um registro, nao o
    // arquivo (HU-35).
    File arquivo = LittleFS.open(CAMINHO_DOS_EVENTOS, "a");
    if (!arquivo) {
      return false;
    }
    const size_t escritos = serializeJson(doc, arquivo);
    // O \n vai junto, e e ele que marca a linha como completa. Linha sem \n e
    // linha que a queda pegou no meio, e a releitura do boot a descarta.
    arquivo.write('\n');
    arquivo.close();

    if (escritos == 0) {
      return false;
    }
  }

  return adicionarNaMemoria(uid);
}

byte quantidade() {
  return total;
}

bool cheia() {
  return total >= CAPACIDADE;
}

size_t imprimirEventos(Print& saida) {
  if (!comArquivo || !LittleFS.exists(CAMINHO_DOS_EVENTOS)) {
    return 0;
  }

  File arquivo = LittleFS.open(CAMINHO_DOS_EVENTOS, "r");
  if (!arquivo) {
    return 0;
  }

  size_t linhas = 0;
  char linha[LINHA_MAX];
  bool completa = false;

  while (lerLinha(arquivo, linha, sizeof(linha), completa)) {
    if (linha[0] == '\0') {
      continue;
    }
    saida.print(linha);
    if (!completa) {
      saida.print("    <- sem quebra de linha: registro pego no meio da escrita");
    }
    saida.println();
    linhas++;
  }

  arquivo.close();
  return linhas;
}

bool apagarEventos() {
  total = 0;
  if (!comArquivo) {
    return true;
  }
  if (!LittleFS.exists(CAMINHO_DOS_EVENTOS)) {
    return true;
  }
  return LittleFS.remove(CAMINHO_DOS_EVENTOS);
}

}  // namespace storage
