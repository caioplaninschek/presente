#include "storage.h"

namespace storage {
namespace {

rfid::Uid registrados[CAPACIDADE];
byte total = 0;

}  // namespace

void iniciarSessao() {
  total = 0;
}

bool jaRegistrado(const rfid::Uid& uid) {
  for (byte i = 0; i < total; i++) {
    if (rfid::mesmoUid(registrados[i], uid)) {
      return true;
    }
  }
  return false;
}

bool registrar(const rfid::Uid& uid) {
  if (jaRegistrado(uid)) {
    return false;
  }
  if (cheia()) {
    return false;
  }
  registrados[total] = uid;
  total++;
  return true;
}

byte quantidade() {
  return total;
}

bool cheia() {
  return total >= CAPACIDADE;
}

}  // namespace storage
