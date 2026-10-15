// presente/relatorio.cpp -- pacote 4 do firmware (#47): o relatorio.
//
// ITEM A ITEM, NUNCA O DOCUMENTO INTEIRO
//
// O relatorio nao cabe num documento fixo (R29). No modo offline o manual
// sempre grava, sem o limite de 64 da memoria de presencas, entao o numero de
// eventos nao tem teto; com 40 alunos online ele ja passa de 8 KB. Por isso a
// exportacao do secao 7 sai em pedacos: a sessao, cada evento lido do
// /eventos.json uma linha por vez (R36) e as matriculas, uma por vez. A mesma
// funcao serve as duas saidas: o Compartilhar escreve direto no celular, e o
// Enviar, no /relatorio.json.
//
// O EVENTO (R37)
//
// Sai da linha do disco: uid, matricula e nome quando a linha os tem, o
// ms_desde_boot trocado pelo timestamp, e a origem. No toque (nfc), enriquecido
// e true quando a linha tem a matricula, que veio da lista da turma, e false
// sem ela; o banco guarda o que recebeu (R33). No manual, lancado_por e motivo,
// sem enriquecido, como no exemplo do secao 7. O evento sem hora se pula, com
// aviso no serial: o banco exige o horario e recusaria o relatorio inteiro.
//
// O FIM
//
// Vem de quem chama, e nullptr tira a chave: o Compartilhar passa o fim do
// Enviar que deu certo, se houve, e o Enviar passa a hora do comeco da
// tentativa, antes de saber se o banco gravou.
//
// Nenhum nome de aluno no serial (R74).

#include "relatorio.h"

#include <Arduino.h>
#include <ArduinoJson.h>
#include <config.h>
#include <storage.h>

#include "chamada.h"
#include "clock.h"
#include "net.h"

namespace relatorio {
namespace {

// O percorrerEventos() pede um ponteiro de funcao, sem captura; dai a saida, a
// virgula e o resultado guardados aqui, como no portal.cpp.
Print* destino = nullptr;
bool primeiroItem = true;
bool inteiro = true;

void escreverTexto(const char* texto) {
  if (destino->print(texto) != strlen(texto)) {
    inteiro = false;
  }
}

void escreverJson(JsonVariantConst valor) {
  if (serializeJson(valor, *destino) != measureJson(valor)) {
    inteiro = false;
  }
}

void escreverItem(JsonVariantConst item) {
  if (!primeiroItem) {
    escreverTexto(",");
  }
  primeiroItem = false;
  escreverJson(item);
}

void escreverEvento(JsonObjectConst evento) {
  char hora[relogio::TAMANHO_DA_HORA];
  const JsonVariantConst ms = evento["ms_desde_boot"];
  if (!ms.is<unsigned long>() || !relogio::formatar(ms.as<unsigned long>(), hora, sizeof(hora))) {
    Serial.println("Evento sem hora: fica fora do relatorio.");
    return;
  }

  // const char* aponta para a linha lida, sem copia-la.
  StaticJsonDocument<256> item;
  const char* uid = evento["uid"];
  const char* matricula = evento["matricula"];
  const char* nome = evento["nome"];
  const char* origem = evento["origem"] | "";
  if (uid != nullptr) {
    item["uid"] = uid;
  }
  if (matricula != nullptr) {
    item["matricula"] = matricula;
  }
  if (nome != nullptr) {
    item["nome"] = nome;
  }
  item["timestamp"] = static_cast<const char*>(hora);
  item["origem"] = origem;
  if (strcmp(origem, "nfc") == 0) {
    item["enriquecido"] = matricula != nullptr;
  } else if (strcmp(origem, "manual") == 0) {
    item["lancado_por"] = evento["lancado_por"].as<const char*>();
    item["motivo"] = evento["motivo"].as<const char*>();  // o null fica null
  }
  escreverItem(item.as<JsonVariantConst>());
}

// As matriculas na ordem da lista da turma: as presentes, ou as que faltam.
void escreverMatriculas(const char* chave, bool presentes) {
  escreverTexto(",\"");
  escreverTexto(chave);
  escreverTexto("\":[");
  primeiroItem = true;
  for (JsonObjectConst aluno : net::alunos()) {
    const JsonVariantConst matricula = aluno["matricula"];
    if (chamada::presente(matricula.as<const char*>()) == presentes) {
      escreverItem(matricula);
    }
  }
  escreverTexto("]");
}

}  // namespace

bool escrever(Print& saida, const char* fim) {
  destino = &saida;
  inteiro = true;

  StaticJsonDocument<256> sessao;
  sessao["sessaoId"] = chamada::sessaoId();
  sessao["professorId"] = chamada::professor();
  sessao["disciplina"] = DISCIPLINA;
  sessao["turma"] = TURMA;
  sessao["inicio"] = chamada::inicio();
  if (fim != nullptr) {
    sessao["fim"] = fim;
  }
  sessao["modo"] = chamada::online() ? "online" : "offline";
  sessao["relogio"] = "portal";

  escreverTexto("{\"sessao\":");
  escreverJson(sessao.as<JsonVariantConst>());
  // O tempos e constante (Q8).
  escreverTexto(",\"tempos\":[\"t1\",\"t2\"],\"roster_carregado\":");
  escreverTexto(chamada::online() ? "true" : "false");
  escreverTexto(",\"eventos\":[");
  primeiroItem = true;
  storage::percorrerEventos(escreverEvento);
  escreverTexto("]");
  // Sem a lista, a API calcula as duas no envio (secao 7).
  if (chamada::online()) {
    escreverMatriculas("presentes", true);
    escreverMatriculas("faltantes", false);
  }
  escreverTexto("}");

  destino = nullptr;
  return inteiro;
}

}  // namespace relatorio
