// lib/storage/storage.h -- quem ja esta registrado na sessao.
//
// Mora em lib/ porque a #26 e a #27 fazem a mesma pergunta antes de registrar
// qualquer cracha (R28: codigo compartilhado sobe para lib/<modulo da secao 5>/).
//
// Este modulo e o embriao do storage.cpp da secao 5 da spec, que lhe atribui os
// professores, a sessao atual, o append de eventos e quem ja esta registrado na
// sessao (R30). Por enquanto so a ultima parte existe, e so na memoria volatil:
// a sessao comeca quando a placa liga e morre com ela.
//
// A #27 e quem poe o LittleFS atras desta mesma pergunta -- o eventos.json
// crescendo por append e relido no boot, para que o cracha gravado antes de uma
// queda de energia continue recusado depois dela. A interface aqui foi escrita
// para aguentar essa troca sem que o programa que chama mude de forma.

#ifndef STORAGE_H
#define STORAGE_H

#include <Arduino.h>
#include <rfid.h>

namespace storage {

// Quantos crachas diferentes cabem numa sessao. Uma turma da UVA nao chega
// perto disso, e o teste 8 da secao 8 da spec usa 30 toques de poucas tags. O
// limite existe porque a memoria e fixa: nada de alocacao dinamica no
// aparelho, pela mesma razao que a spec da para o documento estatico da
// ArduinoJson (R29) -- heap fragmentado derruba o aparelho depois de horas.
const byte CAPACIDADE = 64;

// Esquece tudo. Chamada no boot e, no firmware de verdade, no Encerrar.
void iniciarSessao();

// true se este UID ja foi registrado nesta sessao.
bool jaRegistrado(const rfid::Uid& uid);

// Registra o UID e devolve true. Devolve false se ele ja estava registrado --
// e quem chama decide o que mostrar, que na #26 e uma piscada verde sem som.
// Devolve false tambem quando a lista encheu; cheia() distingue os dois casos.
bool registrar(const rfid::Uid& uid);

byte quantidade();

bool cheia();

}  // namespace storage

#endif  // STORAGE_H
