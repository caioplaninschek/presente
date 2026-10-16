// presente/clock.h -- o relogio da chamada (secao 5 da spec), pacotes 3 e 5
// (#45, #49).
//
// O namespace e relogio, e nao clock (R74): o <time.h>, que o Arduino inclui,
// declara a funcao clock(), e um namespace clock no escopo global nao compila.
//
// Sem conta de fuso (R62, item 3): o portal manda a hora de Brasilia em -03:00,
// e o aparelho guarda os segundos dessa hora como se fosse a de Greenwich e a
// devolve com o mesmo -03:00. Por isso nada de mktime(), que depende do fuso
// do sistema.
//
// UMA ANCORA SO, A DO INICIAR (#49)
//
// O ms_desde_boot de todo evento da chamada conta no relogio do boot em que ela
// foi iniciada. Depois de uma queda, o millis() volta a zero, e o login do boot
// novo traz a hora que mede quanto ele esta atras daquele relogio: dali em
// diante, o agora() devolve o millis() mais esse deslocamento. O formatar()
// continua com a ancora do Iniciar, e quem le os eventos nao muda.

#pragma once

#include <stddef.h>

namespace relogio {

// "AAAA-MM-DDTHH:MM:SS-03:00" e o terminador.
const size_t TAMANHO_DA_HORA = 26;

// Acerta o relogio com a hora do Iniciar, tomada no instante msDesdeBoot, e
// zera o deslocamento: o boot do Iniciar e o da ancora. So aceita o formato
// acima, com o ano entre 2026 e 2099: um celular com o relogio zerado volta
// false, e o relogio fica como estava.
bool acertar(const char* hora, unsigned long msDesdeBoot);

// A retomada (#49): devolve a ancora do Iniciar, lida do /sessao_atual.json, ao
// boot novo. O formatar() volta a valer para os eventos ja gravados, mas o boot
// fica sem hora ate o acertarNaRetomada(). Mesmo formato do acertar().
bool retomar(const char* inicio, unsigned long msDoIniciar);

// O primeiro login do boot novo com hora valida: calcula quanto o millis()
// deste boot esta atras do relogio do Iniciar. Exige a ancora, do acertar() ou
// do retomar(). false com a hora fora do formato, e nada muda.
bool acertarNaRetomada(const char* hora);

// true quando este boot tem hora: houve Iniciar nele, ou o login da retomada.
// Sem ela, nada se grava, porque o horario sairia errado (#49).
bool acertadoNesteBoot();

// O instante de agora no relogio do Iniciar: o millis() mais o deslocamento. E o
// que vai ao ms_desde_boot de todo evento, e o que se passa ao formatar().
unsigned long agora();

// Escreve em destino a hora do instante msDesdeBoot, no formato do acertar().
// false se o relogio ainda nao foi acertado ou se destino e pequeno.
bool formatar(unsigned long msDesdeBoot, char* destino, size_t tamanho);

}  // namespace relogio
