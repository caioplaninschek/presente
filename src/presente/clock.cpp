// presente/clock.cpp -- pacotes 3 e 5 do firmware (#45, #49): a hora do
// Iniciar, a hora de cada evento, para o painel e o relatorio (R74), e o
// relogio do boot que retoma a chamada depois de uma queda.
//
// O arquivo continua com o nome da secao 5; o namespace e relogio (clock.h).
//
// A conta dos dias e a de Howard Hinnant (dias desde 1970 por ano, mes e dia, e
// a volta). Nem mktime() nem gmtime_r(): o primeiro depende do fuso do sistema,
// e o time_t deste toolchain tem 32 bits, que acabam em 2038, dentro dos anos
// que o acertar() aceita.
//
// O DESLOCAMENTO DA RETOMADA (#49)
//
// Com a hora do login do boot novo, e a ancora do Iniciar devolvida pelo
// retomar():
//
//   deslocamento = msNoAcerto + (hora do login - inicio, em s) x 1000 - millis()
//
// e o agora() passa a ser millis() + deslocamento. O formatar() desse instante
// da inicio + (agora - msNoAcerto) / 1000, que e a hora do login mais o tempo
// passado desde ele. Sem o retomar(), o boot novo nao teria ancora nenhuma: o
// acertado volta a false a cada reinicio, e o formatar() recusaria todo evento,
// inclusive os de antes da queda.

#include "clock.h"

#include <Arduino.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

namespace relogio {
namespace {

bool acertado = false;
int64_t segundosNoAcerto = 0;  // desde 1970, contados sem fuso
unsigned long msNoAcerto = 0;

// Quanto o millis() deste boot esta atras do relogio do Iniciar; 0 no proprio
// boot do Iniciar. Com sinal: o celular do login pode estar atrasado.
int32_t deslocamento = 0;
bool comHoraNesteBoot = false;

const char FUSO[] = "-03:00";

int32_t diasDesde1970(int32_t ano, int32_t mes, int32_t dia) {
  ano -= mes <= 2;
  const int32_t era = ano / 400;
  const int32_t anoDaEra = ano - era * 400;
  const int32_t diaDoAno = (153 * (mes > 2 ? mes - 3 : mes + 9) + 2) / 5 + dia - 1;
  const int32_t diaDaEra = anoDaEra * 365 + anoDaEra / 4 - anoDaEra / 100 + diaDoAno;
  return era * 146097 + diaDaEra - 719468;
}

void dataDosDias(int32_t dias, int32_t& ano, int32_t& mes, int32_t& dia) {
  dias += 719468;
  const int32_t era = dias / 146097;  // dias e positivo: o ano vai de 2026 em diante
  const int32_t diaDaEra = dias - era * 146097;
  const int32_t anoDaEra = (diaDaEra - diaDaEra / 1460 + diaDaEra / 36524 - diaDaEra / 146096) / 365;
  const int32_t diaDoAno = diaDaEra - (365 * anoDaEra + anoDaEra / 4 - anoDaEra / 100);
  const int32_t mesDesdeMarco = (5 * diaDoAno + 2) / 153;
  dia = diaDoAno - (153 * mesDesdeMarco + 2) / 5 + 1;
  mes = mesDesdeMarco < 10 ? mesDesdeMarco + 3 : mesDesdeMarco - 9;
  ano = anoDaEra + era * 400 + (mes <= 2);
}

// Le `quantos` digitos a partir de texto. false se algum nao for digito.
bool numero(const char* texto, int quantos, int32_t& valor) {
  valor = 0;
  for (int i = 0; i < quantos; i++) {
    if (texto[i] < '0' || texto[i] > '9') {
      return false;
    }
    valor = valor * 10 + (texto[i] - '0');
  }
  return true;
}

int32_t diasNoMes(int32_t ano, int32_t mes) {
  static const int32_t dias[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  // De 2026 a 2099, todo ano divisivel por 4 e bissexto.
  return mes == 2 && ano % 4 == 0 ? 29 : dias[mes - 1];
}

// Os segundos desde 1970, sem fuso, de uma hora no formato do acertar(). false
// se a hora nao esta no formato ou o ano sai de 2026 a 2099.
bool lerHora(const char* hora, int64_t& segundosDesde1970) {
  if (hora == nullptr || strlen(hora) != TAMANHO_DA_HORA - 1) {
    return false;
  }
  if (hora[4] != '-' || hora[7] != '-' || hora[10] != 'T' || hora[13] != ':' ||
      hora[16] != ':' || strcmp(hora + 19, FUSO) != 0) {
    return false;
  }

  int32_t ano, mes, dia, horas, minutos, segundos;
  if (!numero(hora, 4, ano) || !numero(hora + 5, 2, mes) || !numero(hora + 8, 2, dia) ||
      !numero(hora + 11, 2, horas) || !numero(hora + 14, 2, minutos) ||
      !numero(hora + 17, 2, segundos)) {
    return false;
  }
  if (ano < 2026 || ano > 2099 || mes < 1 || mes > 12 || dia < 1 || dia > diasNoMes(ano, mes) ||
      horas > 23 || minutos > 59 || segundos > 59) {
    return false;
  }

  segundosDesde1970 = static_cast<int64_t>(diasDesde1970(ano, mes, dia)) * 86400 +
                      horas * 3600 + minutos * 60 + segundos;
  return true;
}

}  // namespace

bool acertar(const char* hora, unsigned long msDesdeBoot) {
  int64_t segundos;
  if (!lerHora(hora, segundos)) {
    return false;
  }
  segundosNoAcerto = segundos;
  msNoAcerto = msDesdeBoot;
  deslocamento = 0;
  acertado = true;
  comHoraNesteBoot = true;
  return true;
}

bool retomar(const char* inicio, unsigned long msDoIniciar) {
  if (!acertar(inicio, msDoIniciar)) {
    return false;
  }
  comHoraNesteBoot = false;  // o millis() deste boot ainda nao tem base
  return true;
}

bool acertarNaRetomada(const char* hora) {
  int64_t segundos;
  if (!acertado || !lerHora(hora, segundos)) {
    return false;
  }
  const int64_t calculado = static_cast<int64_t>(msNoAcerto) +
                            (segundos - segundosNoAcerto) * 1000 -
                            static_cast<int64_t>(millis());
  // O formatar() conta a diferenca em 32 bits: uma queda de mais de 24 dias no
  // meio da mesma chamada nao cabe, e o boot fica sem hora.
  if (calculado < INT32_MIN || calculado > INT32_MAX) {
    return false;
  }
  deslocamento = static_cast<int32_t>(calculado);
  comHoraNesteBoot = true;
  return true;
}

bool acertadoNesteBoot() {
  return comHoraNesteBoot;
}

unsigned long agora() {
  return millis() + static_cast<unsigned long>(deslocamento);
}

bool formatar(unsigned long msDesdeBoot, char* destino, size_t tamanho) {
  if (!acertado || tamanho < TAMANHO_DA_HORA) {
    return false;
  }

  // A diferenca com sinal tambem serve a um instante anterior ao acerto, e
  // atravessa a volta do millis() aos 49 dias.
  const int32_t passadosMs = static_cast<int32_t>(msDesdeBoot - msNoAcerto);
  const int64_t segundos = (segundosNoAcerto * 1000 + passadosMs) / 1000;

  const int32_t dias = static_cast<int32_t>(segundos / 86400);
  const int32_t doDia = static_cast<int32_t>(segundos % 86400);
  int32_t ano, mes, dia;
  dataDosDias(dias, ano, mes, dia);

  snprintf(destino, tamanho, "%04d-%02d-%02dT%02d:%02d:%02d%s", static_cast<int>(ano),
           static_cast<int>(mes), static_cast<int>(dia), static_cast<int>(doDia / 3600),
           static_cast<int>(doDia / 60 % 60), static_cast<int>(doDia % 60), FUSO);
  return true;
}

}  // namespace relogio
