// presente/net.h -- o radio do aparelho: o hotspot do professor e o banco
// (secao 5 da spec). Pacote 2 (#43): a busca da lista da turma depois do login.
//
// Nao conhece o portal nem o LED: quem chama pergunta buscando() e decide o
// que mostrar.

#pragma once

#include <ArduinoJson.h>
#include <stddef.h>

namespace net {

// Comeca a busca: apaga o /roster.json antigo e tenta o hotspot. Nao trava;
// quem anda com a busca e o atender(). Chamada com uma busca em curso, nao faz
// nada.
void comecarBusca();

// Precisa ser chamada a cada volta do loop(). So trava durante a conexao
// segura e a leitura da resposta, no maximo os dois limites do config.h.
void atender();

// true do comecarBusca() ate o aparelho sair do hotspot, com ou sem a lista.
bool buscando();

// true quando a ultima busca trouxe a lista (modo online).
bool comLista();

// Alunos na lista; 0 sem ela.
size_t totalDeAlunos();

// A lista da turma como esta na memoria, para a chamada (#45) consultar sem
// copiar: cada aluno com matricula, nome e uid (rpc/roster). Vazia sem a lista.
JsonArrayConst alunos();

// O professor desistiu de esperar (seguir offline): sai do hotspot, sem a
// lista. Sem busca em curso, nao faz nada.
void abandonar();

}  // namespace net
