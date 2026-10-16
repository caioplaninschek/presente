// presente/chamada.h -- a chamada aberta (secao 5 da spec), pacotes 3 a 5
// (#45, #47, #49).
//
// Quem esta presente, a decisao do toque e do lancamento manual, os contadores
// e o ultimo toque. Nao conhece o portal nem o LED (R74): devolve um veredito, e
// quem chama decide o que mostrar, como no net::. O estado SESSAO_ABERTA e do
// portal.cpp.

#pragma once

#include <ArduinoJson.h>
#include <rfid.h>
#include <stddef.h>

namespace chamada {

enum class Abertura { ABERTA, JA_ABERTA, HORA_INVALIDA, NAO_APAGOU };

enum class Retomada { NENHUMA, ABERTA, ENVIADA };

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
// no serial: a chamada abre assim mesmo. Falha ao apagar volta NAO_APAGOU, sem
// mudar nada da chamada (R77). O modo (com ou sem a lista) fica o da abertura.
// Com a chamada iniciada, enviada ou nao, volta JA_ABERTA.
Abertura abrir(const char* hora, const char* professorId, unsigned long msDesdeBoot);

// A retomada depois de uma queda (#49, R16): a chamada volta do
// /sessao_atual.json, ja lido em sessao, com o relogio do Iniciar reacertado e
// os presentes e os eventos relidos do /eventos.json (R30). idDaConta e o id do
// professor do Iniciar no documento global das contas, que vive o boot
// inteiro; nullptr se a conta nao existe mais, e o id lido do arquivo e copiado.
// A lista da turma ja tem de estar carregada (net::carregarLista()) se a
// chamada era online. NENHUMA se o arquivo nao traz uma chamada legivel.
Retomada retomar(JsonObjectConst sessao, const char* idDaConta);

// Houve Iniciar e nao houve Encerrar: a chamada se exibe e se compartilha,
// tambem depois do Enviar (R46).
bool iniciada();

// Iniciada e ainda nao enviada: so assim o toque e o manual registram (#47).
bool aberta();

// "<TURMA>-<AAAAMMDD>-<HHMM>" (secao 7) e a hora do Iniciar, em -03:00.
const char* sessaoId();
const char* inicio();

// O id do professor do Iniciar, para o relatorio nao depender do portal.
const char* professor();

// O modo da abertura: true com a lista da turma (online).
bool online();

// O Enviar que deu certo (#47): a chamada para de registrar e o
// /sessao_atual.json se regrava com os campos do Iniciar, estado "finalizada"
// e o fim (o pacote 5, #49, rele o arquivo). Falha ao gravar so avisa no
// serial.
void fechar(const char* fim);

// A hora do fim do Enviar que deu certo; nullptr antes dele.
const char* fim();

// O Encerrar (#47): esquece a chamada inteira. Os arquivos sao de quem chama.
void zerar();

// O cracha lido. Fora da chamada aberta, NAO_RECONHECIDO sem gravar nada.
Veredito encostou(const rfid::Uid& uid, unsigned long msDesdeBoot);

// O lancamento manual do professor. Em JA_PRESENTE, nome aponta o nome da
// lista; nos outros casos pode ficar nullptr. motivo e opcional. Sem hora neste
// boot (a retomada antes do login com hora, #49), NAO_GRAVADO.
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
