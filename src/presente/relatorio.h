// presente/relatorio.h -- o relatorio da chamada (secao 7 da spec), pacote 4
// (#47).

#pragma once

#include <Print.h>

namespace relatorio {

// Escreve em saida a exportacao do secao 7 da chamada iniciada, item a item. fim
// e a hora do fim da chamada; com nullptr, a chave fim nao sai. Devolve false se
// a saida nao aceitou todos os bytes (a flash cheia, no /relatorio.json).
bool escrever(Print& saida, const char* fim);

}  // namespace relatorio
