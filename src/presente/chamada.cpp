// presente/chamada.cpp -- pacote 3 do firmware (#45): a chamada.
//
// O EVENTO VAI AO DISCO ANTES DA MEMORIA (R30, R36)
//
// Cada toque aceito e cada lancamento manual viram uma linha no /eventos.json,
// e so depois o aluno entra na memoria de presentes. Se a escrita falhar, nada
// e registrado: o aparelho nao passa a achar presente quem o arquivo nao
// conhece, e o relatorio, que sai do arquivo, nao perde ninguem que o painel
// mostrou. O toque que nao gravou recebe o sinal de NAO_RECONHECIDO (procure o
// professor), e o motivo vai para o serial.
//
// A LINHA DO EVENTO (R74)
//
// Toque com a lista: uid, matricula, nome, ms_desde_boot e origem "nfc". Toque
// sem a lista: uid, ms_desde_boot e origem, o formato das bancadas. Manual:
// matricula, nome (so com a lista), ms_desde_boot, origem "manual", lancado_por
// e motivo. O enriquecido e o timestamp sao da saida (R37).
//
// QUEM ESTA PRESENTE (R30)
//
// Com a lista, a presenca se reconhece pela matricula: o UID lido se procura na
// lista, e o aluno lancado a mao que depois encosta o cracha da JA_REGISTRADO.
// Sem a lista, o toque se reconhece pelo UID, e o manual sempre grava: o
// aparelho nao tem como ligar a matricula ao UID, e a API junta os dois no
// envio.
//
// Nenhum nome de aluno no serial (R74): o log vai colado em issue, num
// repositorio publico. O serial leva o UID ou a matricula.

#include "chamada.h"

#include <Arduino.h>
#include <ArduinoJson.h>
#include <config.h>
#include <storage.h>

#include "clock.h"
#include "net.h"

namespace chamada {
namespace {

// A matricula digitada vai ate 20 caracteres (R74), e o UID mais longo tem 20
// digitos: os dois cabem numa chave so.
const size_t MATRICULA_MAX = 20;
const size_t CHAVE_TAMANHO = MATRICULA_MAX + 1;
static_assert(rfid::UID_TEXTO_TAMANHO <= CHAVE_TAMANHO, "o UID precisa caber na chave");

const size_t MOTIVO_MAX = 60;  // em bytes

// A matricula com a lista, o UID sem ela. Memoria fixa, sem alocacao (R29).
// shortcut: 64 presencas por chamada; turma maior pede CAPACIDADE maior
char presencas[storage::CAPACIDADE][CHAVE_TAMANHO];
size_t total = 0;
size_t gravados = 0;

bool estaAberta = false;
bool comLista = false;
char id[40];
char horaDoInicio[relogio::TAMANHO_DA_HORA];

Toque toque;
bool houveToque = false;

bool jaPresente(const char* chave) {
  for (size_t i = 0; i < total; i++) {
    if (strcmp(presencas[i], chave) == 0) {
      return true;
    }
  }
  return false;
}

void marcarPresente(const char* chave) {
  snprintf(presencas[total], CHAVE_TAMANHO, "%s", chave);
  total++;
}

// Procura na lista da turma, sem copia-la.
JsonObjectConst alunoPor(const char* campo, const char* valor) {
  for (JsonObjectConst aluno : net::alunos()) {
    const char* esse = aluno[campo];
    if (esse != nullptr && strcasecmp(esse, valor) == 0) {
      return aluno;
    }
  }
  return JsonObjectConst();
}

bool memoriaCheia(const char* quem) {
  if (total < storage::CAPACIDADE) {
    return false;
  }
  Serial.printf("Memoria de presencas cheia (%u): %s nao registrado.\n", storage::CAPACIDADE, quem);
  return true;
}

bool gravar(const JsonDocument& evento, const char* quem) {
  if (evento.overflowed() || !storage::acrescentarEvento(evento)) {
    Serial.printf("Nao consegui gravar no %s: %s nao registrado.\n", storage::CAMINHO_DOS_EVENTOS, quem);
    return false;
  }
  gravados++;
  return true;
}

Veredito decidir() {
  const char* chave = toque.uid;
  const char* matricula = nullptr;
  if (comLista) {
    const JsonObjectConst aluno = alunoPor("uid", toque.uid);
    matricula = aluno["matricula"];
    if (matricula == nullptr) {
      return Veredito::NAO_RECONHECIDO;  // fora da lista: nada gravado
    }
    toque.nome = aluno["nome"];
    chave = matricula;
  }

  if (jaPresente(chave)) {
    return Veredito::JA_REGISTRADO;
  }
  if (memoriaCheia(toque.uid)) {
    return Veredito::NAO_RECONHECIDO;
  }

  StaticJsonDocument<512> evento;
  evento["uid"] = static_cast<const char*>(toque.uid);
  if (comLista) {
    evento["matricula"] = matricula;
    evento["nome"] = toque.nome;
  }
  evento["ms_desde_boot"] = toque.msDesdeBoot;
  evento["origem"] = "nfc";
  if (!gravar(evento, toque.uid)) {
    return Veredito::NAO_RECONHECIDO;
  }
  marcarPresente(chave);
  return Veredito::REGISTRADO;
}

}  // namespace

Abertura abrir(const char* hora, const char* professorId, unsigned long msDesdeBoot) {
  if (estaAberta) {
    return Abertura::JA_ABERTA;
  }
  if (!relogio::acertar(hora, msDesdeBoot)) {
    return Abertura::HORA_INVALIDA;
  }

  // Do texto da hora ja validada: AAAA, MM, DD, HH e MM (secao 7).
  snprintf(id, sizeof(id), "%s-%.4s%.2s%.2s-%.2s%.2s", TURMA, hora, hora + 5, hora + 8, hora + 11,
           hora + 14);
  snprintf(horaDoInicio, sizeof(horaDoInicio), "%s", hora);
  comLista = net::comLista();
  total = 0;
  gravados = 0;
  houveToque = false;

  // O Iniciar comeca do zero (R74): os arquivos de uma chamada anterior, ou das
  // bancadas, nao entram nesta.
  if (!storage::apagarChamada()) {
    Serial.println("Nao consegui apagar os arquivos da chamada anterior.");
  }

  StaticJsonDocument<384> sessao;
  sessao["sessaoId"] = static_cast<const char*>(id);
  sessao["professorId"] = professorId;
  sessao["turma"] = TURMA;
  sessao["disciplina"] = DISCIPLINA;
  sessao["inicio"] = static_cast<const char*>(horaDoInicio);
  sessao["modo"] = comLista ? "online" : "offline";
  sessao["relogio"] = "portal";
  sessao["ms_desde_boot"] = msDesdeBoot;
  if (!storage::gravarSessao(sessao)) {
    // Recusar o Iniciar deixaria a aula sem chamada; cada toque ainda avisa se
    // nao gravou.
    Serial.printf("Nao consegui gravar o %s: a chamada abre assim mesmo.\n", storage::CAMINHO_DA_SESSAO);
  }

  estaAberta = true;
  Serial.printf("Chamada aberta: %s, modo %s\n", id, comLista ? "online" : "offline");
  return Abertura::ABERTA;
}

bool aberta() {
  return estaAberta;
}

const char* sessaoId() {
  return id;
}

const char* inicio() {
  return horaDoInicio;
}

Veredito encostou(const rfid::Uid& uid, unsigned long msDesdeBoot) {
  if (!estaAberta) {
    return Veredito::NAO_RECONHECIDO;
  }
  rfid::formatarUid(uid, toque.uid, sizeof(toque.uid));
  toque.nome = nullptr;
  toque.msDesdeBoot = msDesdeBoot;
  toque.tipo = decidir();
  houveToque = true;
  return toque.tipo;
}

Lancamento lancar(const char* matricula, const char* motivo, const char* professorId,
                  unsigned long msDesdeBoot, const char*& nome) {
  nome = nullptr;
  if (!estaAberta) {
    return Lancamento::SEM_SESSAO;
  }
  // O limite so protege a memoria: a matricula nao se confere contra o
  // cadastro (R33).
  if (matricula == nullptr || matricula[0] == '\0' || strlen(matricula) > MATRICULA_MAX) {
    return Lancamento::MATRICULA_VAZIA;
  }

  if (comLista) {
    const JsonObjectConst aluno = alunoPor("matricula", matricula);
    if (aluno.isNull()) {
      return Lancamento::FORA_DA_TURMA;
    }
    matricula = aluno["matricula"];  // a da lista, que e a chave da presenca
    nome = aluno["nome"];
    if (jaPresente(matricula)) {
      return Lancamento::JA_PRESENTE;
    }
    if (memoriaCheia(matricula)) {
      return Lancamento::NAO_GRAVADO;
    }
  }

  // Corta em MOTIVO_MAX bytes sem partir uma letra acentuada: o corte nunca cai
  // num byte de continuacao do UTF-8 (10xxxxxx).
  char motivoCortado[MOTIVO_MAX + 1] = "";
  if (motivo != nullptr) {
    size_t n = strlen(motivo);
    if (n > MOTIVO_MAX) {
      n = MOTIVO_MAX;
      while (n > 0 && (static_cast<unsigned char>(motivo[n]) & 0xC0) == 0x80) {
        n--;
      }
    }
    memcpy(motivoCortado, motivo, n);
    motivoCortado[n] = '\0';
  }

  StaticJsonDocument<512> evento;
  evento["matricula"] = matricula;
  if (comLista) {
    evento["nome"] = nome;
  }
  evento["ms_desde_boot"] = msDesdeBoot;
  evento["origem"] = "manual";
  evento["lancado_por"] = professorId;
  if (motivoCortado[0] == '\0') {
    evento["motivo"] = nullptr;
  } else {
    evento["motivo"] = static_cast<const char*>(motivoCortado);
  }
  if (!gravar(evento, matricula)) {
    return Lancamento::NAO_GRAVADO;
  }
  if (comLista) {
    marcarPresente(matricula);
  }
  return Lancamento::ACEITO;
}

bool presente(const char* matricula) {
  return matricula != nullptr && jaPresente(matricula);
}

size_t presentes() {
  return total;
}

size_t eventos() {
  return gravados;
}

const Toque* ultimo() {
  return houveToque ? &toque : nullptr;
}

}  // namespace chamada
