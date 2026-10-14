// presente/clock.h -- o relogio da chamada (secao 5 da spec), pacote 3 (#45).
//
// O namespace e relogio, e nao clock (R74): o <time.h>, que o Arduino inclui,
// declara a funcao clock(), e um namespace clock no escopo global nao compila.
//
// Sem conta de fuso (R62, item 3): o portal manda a hora de Brasilia em -03:00,
// e o aparelho guarda os segundos dessa hora como se fosse a de Greenwich e a
// devolve com o mesmo -03:00. Por isso nada de mktime(), que depende do fuso
// do sistema.

#pragma once

#include <stddef.h>

namespace relogio {

// "AAAA-MM-DDTHH:MM:SS-03:00" e o terminador.
const size_t TAMANHO_DA_HORA = 26;

// Acerta o relogio com a hora do Iniciar, tomada no instante msDesdeBoot. So
// aceita o formato acima, com o ano entre 2026 e 2099: um celular com o relogio
// zerado volta false, e o relogio fica como estava.
bool acertar(const char* hora, unsigned long msDesdeBoot);

// Escreve em destino a hora do instante msDesdeBoot, no formato do acertar().
// false se o relogio ainda nao foi acertado ou se destino e pequeno.
bool formatar(unsigned long msDesdeBoot, char* destino, size_t tamanho);

}  // namespace relogio
