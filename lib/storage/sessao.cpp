// lib/storage/sessao.cpp -- a chamada em disco (#45, R74): o /sessao_atual.json
// (R16) e o /eventos.json do firmware.
//
// Fora do storage.cpp pelo mesmo motivo do roster.cpp: no mesmo arquivo, o
// codigo dos programas de bancada deixaria de sair identico. E sem o
// registrar() de la, cuja linha de 128 bytes so leva uid, ms_desde_boot e
// origem.
//
// Nenhuma funcao daqui monta o LittleFS: o firmware monta com begin(false) no
// main.cpp, e o begin(true) do iniciarSessaoComArquivo() apagaria o portal
// (R41). Pelo mesmo motivo a chamada nao usa o apagarEventos(), que no firmware
// nao apaga nada.

#include "storage.h"

#include <LittleFS.h>

namespace storage {
namespace {

// Uma linha do evento cabe aqui com folga: os campos fixos somam uns 200 bytes,
// e o nome do aluno vem do banco. A linha que nao caberia na releitura nao e
// gravada (acrescentarEvento).
const size_t LINHA_MAX = 512;

// Uma escrita que parou no meio deixa um pedaco sem \n. A linha seguinte comeca
// numa linha nova: colada nele, as duas se perderiam na releitura.
bool linhaPelaMetade = false;

// Le uma linha ate o \n, como o lerLinha() do storage.cpp. Devolve false no fim
// do arquivo; completa diz se a linha terminou em \n de verdade.
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

bool apagar(const char* caminho) {
  return !LittleFS.exists(caminho) || LittleFS.remove(caminho);
}

}  // namespace

bool gravarSessao(const JsonDocument& sessao) {
  File arquivo = LittleFS.open(CAMINHO_DA_SESSAO, "w");
  if (!arquivo) {
    return false;
  }
  const size_t escritos = serializeJson(sessao, arquivo);
  arquivo.close();
  // Flash cheia no meio: o pacote 5 (#49) releria uma sessao cortada.
  if (escritos != measureJson(sessao)) {
    LittleFS.remove(CAMINHO_DA_SESSAO);
    return false;
  }
  return true;
}

bool acrescentarEvento(const JsonDocument& evento) {
  const size_t tamanho = measureJson(evento);
  if (tamanho + 1 > LINHA_MAX) {
    return false;
  }

  // "a" e append: nada do que ja esta escrito e tocado (HU-35).
  File arquivo = LittleFS.open(CAMINHO_DOS_EVENTOS, "a");
  if (!arquivo) {
    return false;
  }
  if (linhaPelaMetade && arquivo.write('\n') != 1) {
    arquivo.close();
    return false;
  }
  linhaPelaMetade = false;
  const size_t escritos = serializeJson(evento, arquivo);
  // O \n marca a linha como completa (R36).
  const size_t quebra = arquivo.write('\n');
  arquivo.close();
  linhaPelaMetade = escritos != tamanho || quebra != 1;
  return !linhaPelaMetade;
}

bool apagarChamada() {
  const bool eventos = apagar(CAMINHO_DOS_EVENTOS);
  if (eventos) {
    linhaPelaMetade = false;
  }
  const bool sessao = apagar(CAMINHO_DA_SESSAO);
  return eventos && sessao;
}

size_t percorrerEventos(void (*aoLer)(JsonObjectConst evento)) {
  if (!LittleFS.exists(CAMINHO_DOS_EVENTOS)) {
    return 0;
  }
  File arquivo = LittleFS.open(CAMINHO_DOS_EVENTOS, "r");
  if (!arquivo) {
    return 0;
  }

  size_t lidas = 0;
  char linha[LINHA_MAX];
  bool completa = false;

  while (lerLinha(arquivo, linha, sizeof(linha), completa)) {
    if (!completa || linha[0] == '\0') {
      continue;  // linha cortada pela queda de energia, ou linha vazia
    }
    // Documento estatico (R29). Lido de um char* que pode ser alterado, o
    // documento aponta para o texto da linha sem copia-lo, e os 512 bytes so
    // guardam a estrutura.
    StaticJsonDocument<512> doc;
    if (deserializeJson(doc, linha) != DeserializationError::Ok) {
      continue;
    }
    const JsonObjectConst evento = doc.as<JsonObjectConst>();
    if (evento.isNull()) {
      continue;
    }
    aoLer(evento);
    lidas++;
  }

  arquivo.close();
  return lidas;
}

}  // namespace storage
