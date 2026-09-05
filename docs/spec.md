# Presente! — SPEC

> Projeto: Dispositivo embarcado para contabilização de presença em sala de aula
> Disciplina: Sistemas Embarcados — UVA Barra, 3ª terça-feira
> Equipe: Gabriel Albuquerque Varela Santarello (1240110815) · Cauã Manuel Proença de Andrade (1240109764) · Igor Rocha Lobato (1240114118) · Caio Parada Oliveira Planinschek (1240205596) · João Victor Berçot Chabudet Cabral (1240108001)
> Status: decisões Q1–Q9 travadas com o Caio em 05/09/2026. Base para a Entrega 03 (07/09 20:00).

## 1. Problema e objetivo

Professores da UVA Barra gastam 10–15 min por aula fazendo chamada manual no site da UVA: tabela com 2 checkboxes por aula (marcadas = falta, desmarcadas = presença), leitura de nome por nome, pergunta "está presente?", marcação manual. Gera perda de tempo de aula, erro de linha (marcar falta no aluno errado), fraude potencial e esforço vocal.

Objetivo do protótipo: terminal embarcado ao lado da porta que registra presença por aproximação de tag NFC, com feedback imediato audiovisual, gestão pelo professor via Wi-Fi próprio do aparelho (sem depender da internet do campus) e relatório JSON exportável + espelho em nuvem gratuita.

Fora do escopo do protótipo (só citado como evolução): catraca, biometria, celular como tag, integração automática com o sistema acadêmico da UVA, backend próprio Node/Python + PostgreSQL auto-hospedado.

## 2. Decisões travadas (grill Q1–Q9)

- Q1 Sensores/atuadores: 2 sensores oficiais (RC522 + RTC DS3231) + 2 atuadores (LED RGB + buzzer ativo 5V). Cumpre o mínimo do projeto integrador (mín. 2 sensores, mín. 1 atuador) com folga no atuador.
- Q2 Tags (simplificado): trabalhar com a tag que vocês JÁ compraram, seja ela qual for. Firmware só LÊ o UID, nunca ESCREVE na tag. O vínculo UID↔matrícula mora no ESP32 (`turma_ativa.json`), não dentro da tag. Por isso Mifare e NTAG215 funcionam do mesmo jeito para presença (só muda o tamanho do UID: 4 vs 7 bytes). Se o kit RC522 veio com cartão/chaveiro branco, esses são Mifare e servem como tags extras de teste — sem comprar nada.
- Q3/Q7 Backend: offline-first com store-and-forward. Fonte da verdade = LittleFS no ESP32. Espelho oportunista = Supabase REST + Postgres (gratuito). Export/download JSON local sempre funciona, com ou sem internet.
- Q4 Auth: até ~10 contas locais em `professores.json`, senha como hash SHA-256 + salt, sessão por cookie. Cada sessão de chamada carrega `professorId`. Troca de senha default no primeiro boot.
- Q5 Semântica: 1 toque = presente. Ausência na janela = falta. Sem controle de saída. Debounce: mesmo UID ignorado por 5 s. Meta de latência: tag → LED+buzzer em <200 ms.
- Q6 Fallback: professor lança matrícula manual pelo portal (`origem: "manual"`, com `lancado_por` + `motivo`). Anti-fraude do "amigo passa 2 tags" = processo (professor fiscaliza) + alerta no dashboard (2 UIDs em <10 s), sem prometer biometria.
- Q8 Tempos UVA: 1 janela de chamada cobre os 2 tempos (`tempos: ["t1","t2"]`, 1 toque = presente nos dois).

## 3. Arquitetura em blocos

```
[Tag Mifare 1K / NTAG215] --RF 13,56 MHz--> [RC522] --SPI VSPI--> [ESP32 DevKit V1]
                                                                |--I2C--> [RTC DS3231] (timestamp real)
                                                                |--GPIO--> [LED RGB] + [Buzzer] (feedback)
                                                                |--SoftAP 192.168.4.1 + Captive Portal (DNS:53 / HTTP:80)--> [Celular do professor (SPA)]
                                                                |--LittleFS--> turma_ativa.json / eventos.json / professores.json
                                                                |--STA oportunista--> POST JSON --> [Supabase REST -> Postgres]
```

Fluxo: professor loga no portal → abre janela de chamada (sessaoId + turma + disciplina) → LED verde fixo = janela aberta → alunos aproximam tag → ESP32 valida UID contra turma_ativa → append em eventos.json + LED verde pisca 2x + bip 100 ms → professor fecha janela → baixa JSON e/ou dispara POST Supabase.

## 4. Componentes (tabela para a Entrega 03)

| Componente | Qtd | Função |
|---|---|---|
| ESP32 DevKit V1 / 32D | 1 | Controlador principal, Wi-Fi AP+STA, WebServer, LittleFS |
| Leitor RFID RC522 13,56 MHz | 1 | Sensor 1 — leitura UID (SPI VSPI: SCK 18, MISO 19, MOSI 23, SS 5, RST 22) |
| RTC DS3231 + bateria CR2032 | 1 | Sensor 2 — timestamp real offline (I2C: SDA 21, SCL 22 — ajustar se conflitar com RST, mover RST p/ 4) |
| Tags NFC 13,56 MHz que vocês já têm (NTAG215 e/ou cartão/chaveiro branco do kit RC522) | 3–5 | Crachás de teste — firmware só lê o UID, nada é gravado na tag |
| LED RGB catodo comum + 3x resistor 220 Ω | 1 | Atuador visual: verde=ok, vermelho=negado/duplicado, azul=janela fechada |
| Buzzer ativo 5V (+ transistor 2N2222 se GPIO não der conta) | 1 | Atuador sonoro: bip 100 ms só no sucesso |
| Protoboard 830 + jumpers + cabo micro-USB / fonte 5V 2A | 1 kit | Montagem e alimentação (RC522 e DS3231 em 3,3V — nunca 5V no RC522) |

Atenção elétrica: RC522 é estritamente 3,3V (alimentar em 5V queima). Buzzer ativo 5V não liga direto em GPIO — usar transistor ou módulo com driver.

## 5. Firmware (C++ Arduino Core — estrutura limpa)

Libs: `MFRC522` + `RTClib` + `DNSServer` + `WebServer` + `LittleFS` + `ArduinoJson` + `HTTPClient` (só p/ Supabase).

Módulos (`src/`): `config.h` (pinos, SSID, tempos) · `storage.cpp` (LittleFS: ler/gravar turma, append evento) · `rfid.cpp` (UID 4/7 bytes, debounce 5 s, valida contra turma) · `rtc.cpp` (hora real, fallback millis) · `portal.cpp` (login, sessão cookie, rotas `/`, `/api/abrir`, `/api/fechar`, `/api/eventos`, `/api/manual`, `/api/export`) · `sync.cpp` (POST Supabase oportunista, fila de retry) · `feedback.cpp` (LED + buzzer, sem `delay()` — máquina de estados com `millis()`).

Regras de performance: loop sem `delay()`; SPI em VSPI hardware; JSON com `ArduinoJson` estático (sem `String` concatenado); log serial com níveis.

Máquina de estados da chamada: `FECHADA (LED azul fixo)` → `ABERTA (LED verde fixo)` → `REGISTRANDO (verde pisca 2x + bip curto 100 ms)` / `DUPLICADO (vermelho curto 300 ms, sem bip, sem novo evento — mesmo UID dentro de 5 s)` / `FORA_DA_TURMA (vermelho longo 1 s + buzz longo ~400 ms, nada é gravado no relatório)` → `FECHADA + relatório pronto`.

Regra FORA_DA_TURMA (decidido 05/09 com o Caio): UID lido mas fora de `turma_ativa.json`, ou janela fechada = feedback de erro longo e nenhum append em `eventos.json`. Tentativas negadas aparecem só como contador volátil no dashboard (debug), nunca no JSON exportado nem no POST Supabase.

## 6. Portal do professor (SPA servida pelo ESP32)

Rotas web: `GET /` (login) → `GET /painel` (abrir/fechar janela, ver contagem presentes/faltantes em tempo real via polling `/api/eventos`, lançar manual, exportar JSON, botão "Enviar p/ nuvem"). Auth: POST login → cookie de sessão com timeout; logout limpa. Upload da turma: POST JSON/CSV (`matricula, nome, uid`) no início da aula — aparelho nunca guarda histórico de todos os alunos (LGPD por design, só a turma da janela).

## 7. Dados (contrato JSON congelado)

`sessao`: `{sessaoId, professorId, disciplina, turma, inicio, fim}`
`evento`: `{uid, matricula, timestamp (RTC ISO-8601 -03:00), origem: "nfc"|"manual", professorId, sessaoId, lancado_por?, motivo?}`
Export: `{sessao, tempos: ["t1","t2"], presentes: [...], faltantes: [...], eventos: [...]}`

Exemplo:
```json
{"sessaoId":"20260907-T1","professorId":"prof02","disciplina":"Sistemas Embarcados","turma":"3a-terca","inicio":"2026-09-07T19:00:00-03:00","tempos":["t1","t2"],"eventos":[
 {"uid":"A391F21B","matricula":"1240205596","timestamp":"2026-09-07T19:05:22-03:00","origem":"nfc","professorId":"prof02"},
 {"matricula":"1240109764","timestamp":"2026-09-07T19:06:10-03:00","origem":"manual","lancado_por":"prof02","motivo":"esqueceu tag"}]}
```

Supabase (espelho): tabelas `sessoes` + `eventos` (colunas espelham o JSON). Chave anon + RLS por `professorId`. ESP32 faz `POST` com `apikey` + `Authorization: Bearer`. Sem internet → evento fica em `eventos.json` e tenta de novo depois (retry com backoff, sem travar o loop).

## 8. Plano de testes (para Entrega 14/09 e 22/09)

1. UID 4 bytes (Mifare) registra <200 ms, 3 aproximações seguidas geram 1 evento (debounce).
2. UID 7 bytes (NTAG215) registra pelo UID, sem escrita.
3. Tag fora da turma → vermelho, sem evento.
4. Sem RTC (bateria fora) → timestamp marcado `rtc: false`, não trava.
5. 2 professores sequenciais → 2 sessões com `professorId` distintos, sem mistura.
6. Sem internet → export local OK, POST enfileira; com internet → Supabase recebe.
7. Stress: 30 toques em sequência, zero duplicata, zero perda após reboot (LittleFS persiste).

## 9. Divisão da equipe (explicação iniciante) + cronograma

Ver seção 9 detalhada na resposta do chat de 05/09 (5 papéis com entregáveis semanais). Resumo: Hardware · Firmware · Portal+Supabase · Estrutura física · Documentação/testes. Cada papel tem 1 dono e 1 revisor.

Cronograma interno: até 07/09 = SPEC + PDF Entrega 03; até 14/09 = firmware v1 (RC522+Mifare+RTC+LittleFS) + portal login/abrir/fechar/export; até 21/09 = AP+STA + Supabase + LED/buzzer finais + testes 1–7 documentados; 22/09 = protótipo eletrônico; 29/09 = caixa/suporte; 05/10 = dashboard final; 20/10 = vídeo 2 min.

## 10. Desafio técnico principal (pergunta obrigatória da Entrega 03)

Manter o loop responsivo (<200 ms) conciliando 4 coisas que disputam o ESP32: leitura SPI do RC522 com UIDs de 2 tamanhos (4 vs 7 bytes), tempo real offline (RTC com fallback), portal Wi-Fi em AP+STA sem bloquear a antena, e escrita append-only no LittleFS sem corromper em queda de energia — tudo com debounce que filtra repetição sem criar fila na porta.

## 11. Glossário mínimo

Tag = crachá físico. UID = número de série do chip (4 ou 7 bytes). Matrícula = vínculo UID↔aluno (vive em turma_ativa, não no chip, exceto Mifare de teste). Sessão de chamada = janela aberta pelo professor (tem professorId + turma + início/fim). Evento = 1 registro (nfc ou manual). Relatório JSON = contrato de exportação (1 janela = 2 tempos presentes). Debounce = ignorar repetição do mesmo UID por 5 s. Latência = tag→bip+LED.

## 12. Referências (para citar na Entrega 03)

OLIVEIRA & ANDRADE (2010) — hardware/firmware, SPI, temporização, drivers. GUIMARÃES (2009) — sensores/atuadores, eletrônica embarcada. YAGHMOUR et al. (2009) — sistemas embarcados, tempo e integração (uso conceitual, firmware aqui é bare-metal Arduino Core, não Linux).
