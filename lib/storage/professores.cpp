// lib/storage/professores.cpp -- a leitura do professores.json (#41).
//
// Fora do storage.cpp de proposito: no mesmo arquivo, o compilador otimiza de
// outro jeito o leitor de JSON que os programas de bancada ja usam, e o codigo
// deles deixa de sair identico. Aqui, quem nao chama lerProfessores() nem o
// recebe na imagem.

#include "storage.h"

#include <LittleFS.h>

namespace storage {

DeserializationError lerProfessores(JsonDocument& destino) {
  if (!LittleFS.exists(CAMINHO_DOS_PROFESSORES)) {
    return DeserializationError::EmptyInput;
  }
  File arquivo = LittleFS.open(CAMINHO_DOS_PROFESSORES, "r");
  if (!arquivo) {
    return DeserializationError::EmptyInput;
  }
  const DeserializationError erro = deserializeJson(destino, arquivo);
  arquivo.close();
  return erro;
}

}  // namespace storage
