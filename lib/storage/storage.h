// lib/storage/storage.h -- quem ja esta registrado na sessao, e o eventos.json.
//
// Mora em lib/ porque a #26 e a #27 fazem a mesma pergunta antes de registrar
// qualquer cracha (R28: codigo compartilhado sobe para lib/<modulo da secao 5>/).
//
// Este modulo e o embriao do storage.cpp da secao 5 da spec. Dela, ja existem o
// append de eventos e quem ja esta registrado na sessao (R30). Os professores e
// o sessao_atual.json (R16) nao -- o segundo e lacuna declarada no secao 8.
//
// ---------------------------------------------------------------------------
// O FORMATO DO ARQUIVO -- uma linha por evento, nao um array
//
// O eventos.json NAO e um documento JSON unico. Cada linha e um objeto JSON
// completo, terminada em \n, e o arquivo cresce so por append. O objeto de
// Exportacao do secao 7, com os eventos dentro de um array, e montado na saida
// -- no Exibir, no Compartilhar e no Enviar --, nunca guardado assim.
//
// O porque: manter um array valido em disco obriga a reescrever o fecho a cada
// evento, e uma queda de energia no meio dessa reescrita deixa o arquivo inteiro
// impossivel de abrir. Trocaria a perda de um registro pela perda de todos, que
// e exatamente o contrario da HU-35. Com linhas, uma queda deixa no maximo uma
// ultima linha sem o \n, e a releitura do boot a descarta.
//
// Isto nao contraria o secao 7, que e contrato de saida e nao formato em disco:
// o secao 11 ja define evento como "uma linha do eventos.json" e o secao 10 ja
// chama a escrita de append-only. Decidido na issue #27 pelo Caua; falta virar
// R<n> no secao 2 da spec.
//
// O campo de horario chama-se ms_desde_boot e NAO timestamp. O timestamp do
// secao 7 e ISO-8601 com fuso e vem do navegador do professor no Iniciar, que
// ainda nao existe (clock.cpp, secao 5). Gravar milissegundos numa chave
// chamada timestamp faria o contrato mentir para quem lesse o arquivo.
// ---------------------------------------------------------------------------

#ifndef STORAGE_H
#define STORAGE_H

#include <Arduino.h>
#include <rfid.h>

namespace storage {

// Quantos crachas diferentes cabem numa sessao. Uma turma da UVA nao chega
// perto disso, e o teste 8 da secao 8 da spec usa 30 toques de poucas tags. O
// limite existe porque a memoria e fixa: nada de alocacao dinamica no aparelho,
// pela mesma razao que a spec da para o documento estatico da ArduinoJson
// (R29) -- heap fragmentado derruba o aparelho depois de horas.
const byte CAPACIDADE = 64;

const char CAMINHO_DOS_EVENTOS[] = "/eventos.json";

// Sessao que morre com a placa: a lista fica so na memoria e nada e gravado.
// E o que a #26 usa, onde "ja registrado" quer dizer "desde o boot".
void iniciarSessaoNaMemoria();

// Sessao com o eventos.json atras dela (#27). Monta o LittleFS, rele o arquivo
// e reconstroi a lista de quem ja esta registrado, que e o que impede o cracha
// gravado antes de uma queda de energia de ser gravado de novo depois dela.
//
// Devolve quantos eventos reconheceu, ou -1 se o LittleFS nao montou. Uma
// ultima linha sem \n e descartada e nao entra na conta: e o registro que a
// queda de energia pegou no meio da escrita.
int iniciarSessaoComArquivo();

// true quando a sessao corrente grava no arquivo.
bool persistindo();

bool jaRegistrado(const rfid::Uid& uid);

// Registra o UID. Devolve false se ele ja estava registrado -- e quem chama
// decide o que mostrar, que na #26 e uma piscada verde sem som --, se a lista
// encheu, ou se a gravacao no arquivo falhou.
//
// Com o arquivo atras, a linha e escrita ANTES de o UID entrar na memoria: se a
// gravacao falhar, o aparelho nao passa a achar que registrou alguem que o
// arquivo nao conhece.
//
// msDesdeBoot vai na linha como ms_desde_boot. Numa sessao so de memoria ele
// nao e usado.
bool registrar(const rfid::Uid& uid, unsigned long msDesdeBoot);

byte quantidade();

bool cheia();

// Despeja o arquivo cru na saida, linha por linha, e devolve quantas linhas
// saiu. E o passo 4 do roteiro de bancada da #27.
size_t imprimirEventos(Print& saida);

// Apaga o eventos.json e zera a lista, para recomecar o teste do zero.
bool apagarEventos();

}  // namespace storage

#endif  // STORAGE_H
