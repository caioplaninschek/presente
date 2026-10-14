// presente/clock.cpp -- pacote 3 do firmware (#45): a hora do Iniciar e a hora
// de cada evento, para o painel e o relatorio (R74).
//
// O arquivo continua com o nome da secao 5; o namespace e relogio (clock.h).
//
// A conta dos dias e a de Howard Hinnant (dias desde 1970 por ano, mes e dia, e
// a volta). Nem mktime() nem gmtime_r(): o primeiro depende do fuso do sistema,
// e o time_t deste toolchain tem 32 bits, que acabam em 2038, dentro dos anos
// que o acertar() aceita.

#include "clock.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

namespace relogio {
namespace {

bool acertado = false;
int64_t segundosNoAcerto = 0;  // desde 1970, contados sem fuso
unsigned long msNoAcerto = 0;

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

}  // namespace

bool acertar(const char* hora, unsigned long msDesdeBoot) {
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

  segundosNoAcerto = static_cast<int64_t>(diasDesde1970(ano, mes, dia)) * 86400 +
                     horas * 3600 + minutos * 60 + segundos;
  msNoAcerto = msDesdeBoot;
  acertado = true;
  return true;
}

bool formatar(unsigned long msDesdeBoot, char* destino, size_t tamanho) {
  if (!acertado || tamanho < TAMANHO_DA_HORA) {
    return false;
  }

  // A diferenca com sinal tambem serve a um instante anterior ao acerto, e
  // atravessa a volta do millis() aos 49 dias.
  const int32_t passadosMs = static_cast<int32_t>(msDesdeBoot - msNoAcerto);
  const int64_t agora = (segundosNoAcerto * 1000 + passadosMs) / 1000;

  const int32_t dias = static_cast<int32_t>(agora / 86400);
  const int32_t doDia = static_cast<int32_t>(agora % 86400);
  int32_t ano, mes, dia;
  dataDosDias(dias, ano, mes, dia);

  snprintf(destino, tamanho, "%04d-%02d-%02dT%02d:%02d:%02d%s", static_cast<int>(ano),
           static_cast<int>(mes), static_cast<int>(dia), static_cast<int>(doDia / 3600),
           static_cast<int>(doDia / 60 % 60), static_cast<int>(doDia % 60), FUSO);
  return true;
}

}  // namespace relogio
