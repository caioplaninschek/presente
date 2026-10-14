// presente/chamada.h -- a chamada aberta (secao 5 da spec), pacote 3 (#45).
//
// Quem esta presente, a decisao do toque e do lancamento manual, os contadores
// e o ultimo toque. Nao conhece o portal nem o LED (R74): devolve um veredito, e
// quem chama decide o que mostrar, como no net::. O estado SESSAO_ABERTA e do
// portal.cpp.

#pragma once

#include <rfid.h>
#include <stddef.h>

namespace chamada {

enum class Abertura { ABERTA, JA_ABERTA, HORA_INVALIDA };

enum class Veredito { REGISTRADO, JA_REGISTRADO, NAO_RECONHECIDO };

enum class Lancamento { ACEITO, SEM_SESSAO, MATRICULA_VAZIA, FORA_DA_TURMA, JA_PRESENTE, NAO_GRAVADO };

struct Toque {
  Veredito tipo;
  char uid[rfid::UID_TEXTO_TAMANHO];
  const char* nome;       // da lista da turma; nullptr sem ela ou fora dela
  const char* matricula;  // idem, para o serial, que nao leva nome (R74)
  unsigned long msDesdeBoot;
};

// O Iniciar: acerta o relogio com a hora do celular, apaga os arquivos de uma
// chamada anterior e grava o /sessao_atual.json (R16). Falha ao gravar so avisa
// no serial: a chamada abre assim mesmo. O modo (com ou sem a lista) fica o da
// abertura.
Abertura abrir(const char* hora, const char* professorId, unsigned long msDesdeBoot);

bool aberta();

// "<TURMA>-<AAAAMMDD>-<HHMM>" (secao 7) e a hora do Iniciar, em -03:00.
const char* sessaoId();
const char* inicio();

// O cracha lido. Fora da chamada aberta, NAO_RECONHECIDO sem gravar nada.
Veredito encostou(const rfid::Uid& uid, unsigned long msDesdeBoot);

// O lancamento manual do professor. Em JA_PRESENTE, nome aponta o nome da
// lista; nos outros casos pode ficar nullptr. motivo e opcional.
Lancamento lancar(const char* matricula, const char* motivo, const char* professorId,
                  unsigned long msDesdeBoot, const char*& nome);

// Com a lista, se a matricula ja esta presente.
bool presente(const char* matricula);

// Presencas na memoria: alunos com a lista, crachas sem ela.
size_t presentes();

// Eventos gravados no /eventos.json: toques aceitos e lancamentos manuais.
size_t eventos();

// nullptr antes do primeiro toque da chamada. O manual nao conta (secao 6.1).
const Toque* ultimo();

}  // namespace chamada
