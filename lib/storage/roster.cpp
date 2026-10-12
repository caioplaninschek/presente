// lib/storage/roster.cpp -- a lista da turma em disco, o /roster.json (#43).
//
// Fora do storage.cpp pelo mesmo motivo do professores.cpp: no mesmo arquivo,
// o codigo dos programas de bancada deixaria de sair identico.
//
// Reler o arquivo depois de uma queda de energia e do pacote 5 (#49, R16), e
// apaga-lo no Encerrar, do pacote 4 (#47).

#include "storage.h"

#include <LittleFS.h>

namespace storage {

bool gravarRoster(const JsonDocument& lista) {
  File arquivo = LittleFS.open(CAMINHO_DO_ROSTER, "w");
  if (!arquivo) {
    return false;
  }
  const size_t escritos = serializeJson(lista, arquivo);
  arquivo.close();
  // Flash cheia no meio: um arquivo cortado faria o pacote 5 reler lixo.
  if (escritos != measureJson(lista)) {
    LittleFS.remove(CAMINHO_DO_ROSTER);
    return false;
  }
  return true;
}

bool apagarRoster() {
  return !LittleFS.exists(CAMINHO_DO_ROSTER) || LittleFS.remove(CAMINHO_DO_ROSTER);
}

}  // namespace storage
