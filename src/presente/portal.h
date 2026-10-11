// presente/portal.h -- o servidor da rede do aparelho: as paginas do portal do
// professor, o login e as rotas do secao 6.1 da spec (R62).

#pragma once

#include <ArduinoJson.h>

namespace portal {

// Registra as rotas e liga o servidor na porta 80. professores e o documento
// que o main.cpp leu do professores.json; vazio, todo login recebe 401.
// littlefsMontado diz se ha de onde servir as paginas.
void iniciar(const JsonDocument& professores, bool littlefsMontado);

// Precisa ser chamada a cada volta do loop().
void atender();

}  // namespace portal
