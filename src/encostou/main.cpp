// encostou/main.cpp -- programa da issue #26.
//
// Junta o leitor e o feedback: o aluno encosta o cracha, o LED pisca verde e o
// buzzer apita. Cobre os testes 1 e 2 da secao 8 da spec.
//
// O comportamento e o da maquina de estados da secao 5, na parte que existe sem
// a lista da turma dentro do aparelho:
//
//   primeira leitura do cracha  -> verde, 2 piscadas, bip de 100 ms, registra
//   cracha ja registrado        -> verde, 1 piscada, sem som, NAO registra (R30)
//   mesmo cracha dentro de 5 s  -> nada acontece, nem verde nem som (R30)
//
// O cracha que nao e da turma nao entra aqui: o estado NAO_RECONHECIDO so existe
// em modo online, e a lista da turma e outra frente.
//
// A sessao e a placa ligada: "ja registrado" quer dizer registrado desde o boot.
// Quem faz a lista sobreviver ao reinicio e a #27, lendo o eventos.json.
//
// O mecanismo nao mora aqui. A leitura e a janela de silencio estao em
// lib/rfid/, o LED e o buzzer em lib/feedback/, a lista de registrados em
// lib/storage/ (R28). Aqui esta so a ligacao entre os tres -- qual sinal
// corresponde a qual situacao.
//
// LED que nao acende nenhuma cor: a peca e de anodo comum, e a inversao e uma
// linha so -- a constante CATODO_COMUM em lib/feedback/feedback.cpp.
//
// Loop sem delay(): as piscadas e o bip sao temporizados por millis(), senao o
// aparelho congela durante o feedback e perde a tag seguinte.

#include <Arduino.h>
#include <feedback.h>
#include <rfid.h>
#include <storage.h>

// Tempos do feedback. Valores de partida: a secao 8 da spec diz que calibrar
// faz parte do teste, com o aparelho montado na caixa.
const unsigned long BIP_MS = 100;
const unsigned long PISCADA_ACESA_MS = 90;
const unsigned long PISCADA_APAGADA_MS = 90;

// O alvo da secao 5 para o intervalo do encostar ate o feedback.
const unsigned long ALVO_MS = 200;

// De quanto em quanto tempo o aviso de leitor mudo se repete: o monitor serial
// costuma abrir depois do boot e comeria a linha impressa uma vez so.
const unsigned long INTERVALO_DO_AVISO_MS = 3000;

bool leitorVivo = false;
unsigned long ultimoAviso = 0;

void avisarLeitorMudo() {
  const byte versao = rfid::versaoDoLeitor();
  Serial.print("RC522 nao respondeu - VersionReg = 0x");
  if (versao < 0x10) {
    Serial.print("0");
  }
  Serial.println(versao, HEX);
  Serial.println("  Confira 3,3 V no RC522, o RST no pino 22 e o SDA no pino 5.");
}

void imprimirLinha(const char* uidTexto, byte tamanho, const char* veredito,
                   unsigned long leituraAteFeedbackUs,
                   unsigned long toqueAteFeedbackUs) {
  Serial.print(uidTexto);
  Serial.print("  ");
  Serial.print(tamanho);
  Serial.print(" bytes  ");
  Serial.print(veredito);

  // Dois numeros, porque eles medem coisas diferentes e so o segundo responde
  // ao alvo da spec.
  //
  // "decidir" e o que a issue #26 pede: da leitura ter voltado ate o feedback
  // comecar. Sai perto de zero, porque no meio ha uma consulta a um vetor.
  //
  // "toque->feedback" comeca antes da conversa com o RC522, que e onde o tempo
  // de verdade esta. E este que responde ao alvo da secao 5, e e ele que decide
  // se forma fila na porta da sala.
  //
  // Ele e um piso, nao o numero exato: o que a tag esperou no campo ate o loop()
  // vir busca-la nao da para medir de dentro do aparelho. Como o loop() nao tem
  // delay(), essa espera e o tempo de uma volta -- pequena aqui, e e por isso
  // que a regra de nao bloquear vale desde o primeiro programa.
  Serial.print("  decidir: ");
  Serial.print(leituraAteFeedbackUs / 1000.0, 1);
  Serial.print(" ms  toque->feedback: ");
  Serial.print(toqueAteFeedbackUs / 1000.0, 1);
  Serial.print(" ms");

  if (toqueAteFeedbackUs > ALVO_MS * 1000UL) {
    Serial.print("  ACIMA DOS ");
    Serial.print(ALVO_MS);
    Serial.print(" ms");
  }
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  rfid::iniciar();
  feedback::iniciar();
  storage::iniciarSessaoNaMemoria();

  Serial.println();
  Serial.println("Presente! - encostou: verde, bip, e nada de registrar duas vezes (issue #26)");

  ultimoAviso = millis();
  leitorVivo = rfid::leitorRespondeu();
  if (!leitorVivo) {
    avisarLeitorMudo();
    return;
  }

  Serial.println("Sessao aberta com a placa. Encoste uma tag.");
  Serial.println("decidir = da leitura ao feedback; toque->feedback = inclui a conversa com o leitor (alvo: 200 ms)");
}

void loop() {
  // Conduz as piscadas e desliga o buzzer na hora certa. Sem esta linha o
  // feedback comeca e nao termina.
  feedback::atualizar();

  // Leitor mudo: nao adianta procurar tag. Repete o diagnostico e espera que
  // alguem mexa na fiacao e aperte o reset.
  if (!leitorVivo) {
    if (millis() - ultimoAviso >= INTERVALO_DO_AVISO_MS) {
      ultimoAviso += INTERVALO_DO_AVISO_MS;
      avisarLeitorMudo();
    }
    return;
  }

  rfid::Uid uid;
  if (!rfid::lerTagNova(uid)) {
    return;
  }
  const unsigned long leituraVoltouEm = micros();

  // A decisao. E aqui que a R30 vive: quem ja esta registrado nao se registra de
  // novo em momento nenhum, e o sinal disso e verde de uma piscada, sem som.
  const bool primeiraVez = storage::registrar(uid, millis());

  if (primeiraVez) {
    feedback::piscar(feedback::Cor::VERDE, 2, PISCADA_ACESA_MS, PISCADA_APAGADA_MS);
    feedback::bipar(BIP_MS);
  } else {
    feedback::piscar(feedback::Cor::VERDE, 1, PISCADA_ACESA_MS, PISCADA_APAGADA_MS);
  }
  const unsigned long feedbackComecouEm = micros();

  char uidTexto[rfid::UID_TEXTO_TAMANHO];
  rfid::formatarUid(uid, uidTexto, sizeof(uidTexto));

  const char* veredito = primeiraVez ? "registrado   " : "ja registrado";
  imprimirLinha(uidTexto, uid.tamanho, veredito,
                feedbackComecouEm - leituraVoltouEm,
                feedbackComecouEm - rfid::inicioDaLeituraUs());

  // A lista cheia nao acontece numa turma nem no teste 8, mas se acontecer
  // precisa sair no log em vez de passar em silencio. O aviso sai no evento que
  // ENCHE a lista, o 64o cracha, que ainda entra; do 65o em diante registrar()
  // devolve false por lista cheia e o cracha novo recebe o sinal de
  // JA_REGISTRADO -- uma piscada verde, sem som -- sem ter sido registrado.
  // O limite e deste programa de bancada: a #27 poe o arquivo no lugar do vetor.
  if (primeiraVez && storage::cheia()) {
    Serial.println("AVISO: a lista de registrados encheu; os proximos crachas nao entram.");
  }
}
