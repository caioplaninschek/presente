# Presente! — SPEC

> Projeto: Terminal embarcado para contabilização de presença em sala de aula
> Disciplina: Sistemas Embarcados — UVA Barra, 3ª terça-feira
> Equipe: Gabriel Albuquerque Varela Santarello (1240110815) · Cauã Manuel Proença de Andrade (1240109764) · Igor Rocha Lobato (1240114118) · Caio Parada Oliveira Planinschek (1240205596) · João Victor Berçot Chabudet Cabral (1240108001)
> Status: rodada 1 (Q1–Q9) travada em 05/09/2026 de madrugada. Rodada 2 (R1–R8) em 05/09/2026 12h — arquitetura de dados, sensor 2 e rede. **Rodada 3 (R9–R12) em 06/09/2026 00h** — ferramentas e processo. Base para a Entrega 03 (07/09, 20:00).

## 1. Problema e objetivo

Professores da UVA Barra gastam 10–15 min por aula fazendo chamada manual no site da UVA: tabela com 2 checkboxes por aula (marcadas = falta, desmarcadas = presença), leitura de nome por nome, pergunta "está presente?", marcação manual. Gera perda de tempo de aula, erro de linha (marcar falta no aluno errado), fraude potencial e esforço vocal.

Objetivo do protótipo: terminal embarcado **fixo ao lado da porta** que registra presença por aproximação de tag NFC, com feedback imediato audiovisual, gestão pelo professor via Wi-Fi próprio do aparelho e relatório JSON que sobe para uma API na nuvem — com exportação local garantida quando não houver internet.

Fora do escopo do protótipo (citado só como evolução na documentação): catraca, biometria, celular como tag, integração real com o sistema acadêmico da UVA, backend próprio auto-hospedado, controle de saída da sala.

## 2. Decisões travadas

### Rodada 1 — Q1–Q9 (05/09, madrugada)

- **Q2 Tags:** firmware só **LÊ** o UID, nunca escreve na tag. Mifare (4 bytes) e NTAG215 (7 bytes) funcionam igual. Trabalhar com as tags já compradas; cartões brancos do kit RC522 servem de teste extra.
- **Q4 Auth:** até ~10 contas locais em `professores.json`, senha como hash SHA-256 + salt, sessão por cookie. Cada sessão carrega `professorId`. Troca de senha default no primeiro boot.
- **Q5 Semântica:** 1 toque = presente. Ausência = falta. Debounce: mesmo UID ignorado por 5 s. Meta de latência: tag → LED+buzzer em **<200 ms**.
- **Q6 Fallback:** professor lança matrícula manual pelo portal (`origem: "manual"`, com `lancado_por` + `motivo`).
- **Q8 Tempos UVA:** 1 sessão de chamada cobre os 2 tempos (`tempos: ["t1","t2"]`, 1 toque = presente nos dois).

### Rodada 2 — R1–R8 (05/09, 12h) — **revisa a rodada 1**

- **R1 Cadastro de tag sai do aparelho.** A tag chega ao aluno **já cadastrada** — pela equipe no protótipo, pela instituição no cenário real. O professor nunca cadastra ninguém. Consequência: **`turma_ativa.json` deixa de existir**; a lista da turma passa a vir da API.
- **R2 Sincronização em 2 momentos, nunca por toque.** ❌ Descartado: uma chamada de API por aluno (40 alunos = 40 idas à internet durante a aula, latência refém do Wi-Fi, fila na porta). ✅ Adotado:
  - **Iniciar** → 1 chamada: baixa a lista da turma (matrícula + nome + UID, ~3 KB p/ 40 alunos) e cacheia no LittleFS pela sessão.
  - **Durante a aula** → zero internet. Consulta local, <200 ms garantido.
  - **Enviar** → 1 chamada: sobe o relatório completo.
- **R3 Modo offline degradado.** Sem internet no Iniciar, o aparelho entra em **modo UID-cru**: registra só os números das tags, marca `modo: "offline"`, e o enriquecimento (nome/matrícula) acontece no envio. Nada se perde. Consequência: **offline o aparelho não sabe validar turma** — aceita qualquer UID lido, e o backend julga depois.
- **R4 AP e STA nunca simultâneos.** O ESP32 tem uma antena só; ser hotspot e cliente ao mesmo tempo derruba o portal do professor (o softAP muda de canal para acompanhar o STA). Como só existem 2 momentos de internet, o rádio **alterna**: `AP → STA → AP`. ⚠️ **Corrigido em R13:** a transição leva **5–15 s**, não os ~3 s estimados aqui.
- **R5 Internet vem de hotspot de celular**, com SSID e senha configuráveis pelo próprio portal. ⚠️ O ESP32 **não** passa por portal cativo nem autenticação corporativa, então a rede da UVA provavelmente está fora. Documentar: "em produção a instituição provisionaria rede dedicada aos dispositivos".
- **R6 Duas camadas de senha, não uma.**
  - **WPA2 no SoftAP** (mesma para todos, não é segredo, pode estar colada na caixa) → criptografa o ar. Sem ela a senha do professor trafega legível na sala.
  - **Login individual por professor** (hash local) → identidade. Continua um por professor, como na Q4.
- **R7 Sensor 2 = SW-420 (vibração) como anti-violação.** ❌ Descartado o **RTC DS3231**: é redundante (o navegador do professor entrega a hora no Iniciar; a API também) e discutível como "sensor". ❌ Descartado sensor de porta (PIR/ultrassônico): outro grupo da turma faz projeto similar com sensor de porta. ✅ SW-420: o aparelho é **fixo, sozinho na sala e guarda dados de presença** — um aluno tem motivo para mexer nele. Impacto/violação vira alerta no relatório. ⚠️ Módulo é ruidoso: exige debounce em software **e** calibração do potenciômetro de limiar já montado no lugar definitivo.
- **R8 Sem controle de saída** (confirma Q5, **diverge do Canvas da Entrega 02**, que prometia "1º toque entrada, 2º toque saída"). Motivo: duplica estado, duplica falha e não resolve o problema. **A Entrega 03 tem que assumir a mudança em uma linha explícita**, senão o professor lê como incoerência.

### Rodada 3 — R9–R12 (06/09, 00h) — ferramentas e processo

- **R9 Build em PlatformIO** (VS Code), não Arduino IDE. O `platformio.ini` fixa as versões de biblioteca para os 5 integrantes; divergência de versão entre máquinas é o bug que mais custa tempo em equipe.
- **R10 A SPA do portal mora em arquivos no LittleFS**, não como string embutida no `.cpp`. Permite ajustar o HTML e subir para a placa sem recompilar o firmware — é o que deixa portal e firmware avançarem em paralelo.
- **R11 Credenciais fora do git.** `secrets.h` no `.gitignore` (chave do Supabase, SSID/senha do hotspot) + `secrets.example.h` versionado com os campos em branco. Protótipo acadêmico vaza chave em apresentação e em relatório final; 10 minutos de prevenção.
- **R12 O repo sobe para o GitHub e é o diário de bordo.** Os 5 sabem usar. Commits datados são a prova de evolução semanal que o professor exige; `DIARIO.md` na raiz é a versão legível, atualizada semanalmente. Regra registrada no `AGENTS.md`.

### Rodada 4 — R13 (06/09, 00h30) — validada por pesquisa

Fecha o ticket *Caminho viável do ESP32 até o Supabase*. Detalhe completo e fontes em `.scratch/presente/issues/03-esp32-https-supabase.md`.

- **R13 Transporte: `HTTPClient` + `WiFiClientSecure` + `setInsecure()`.** Sem biblioteca de Supabase, sem NTP, sem certificado pinado.
  - **Por que não pinar:** a Supabase não usa uma CA só e troca de emissor sem aviso (verificado em 06/09/2026: Let's Encrypt, Amazon e Google servindo endpoints diferentes). Certificado pinado quebra sozinho e ninguém saberia por quê.
  - **Custo assumido:** `setInsecure()` criptografa mas **não autentica o servidor** — MITM teoricamente possível no hotspot. Proporcional (hotspot do próprio grupo, chave anon descartável, protótipo acadêmico). **Registrar essa limitação no relatório técnico**, não escondê-la.
  - **Sem a lib `ESPSupabase`:** parada desde 07/2025, arrasta `WebSockets` como dependência morta, e internamente só faz o que 20 linhas próprias fazem.
- **R13a Fechar o AP antes de abrir o TLS é requisito de memória.** O handshake pede 40–50 KB de heap livre (mbedTLS aloca 16 KB RX + 16 KB TX). R4 deixa de ser só questão de canal de rádio.
- **R13b Limitar as tentativas de `connect()`.** `WiFiClientSecure` vaza ~4 KB de heap por conexão **falha** (arduino-esp32 #3808); retry sem limite trava o aparelho.
- **R13c `getStream()` no GET do roster**, nunca `getString()` — `getString()` em resposta de ~3 KB fragmenta o heap.
- **R13d Checklist de hotspot** (vai impresso para a apresentação):
  - **iPhone:** ligar *"Maximizar Compatibilidade"* no Ponto de Acesso (força 2,4 GHz); manter a tela do hotspot aberta durante o pareamento (o iOS suspende o beacon); nome do iPhone sem acento nem caractere especial, porque vira o SSID.
  - **Android:** escolher a banda 2,4 GHz explicitamente; senha sem caractere especial.
  - Gravar um **SSID de fallback** no aparelho antes do dia da apresentação.

### Rodada 5 — R14–R17 (06/09, 01h)

- **R14 A sincronização acontece no login, não no Iniciar.** O aparelho valida a senha, responde *"carregando turma…"*, e só então troca o rádio. Quando o painel abre, o roster está em cache e o **Iniciar** é instantâneo. A espera de 5–15 s não some — muda para o instante em que o professor já espera que a página demore. Altera o §6.
  - Alternativa considerada e **descartada por razão acadêmica**: tirar a internet do ESP32 e deixar o celular do professor falar com o Supabase (o aparelho nunca sairia do modo AP). Mataria a troca de rádio, o bug do AP, o TLS e 40–50 KB de heap — mas numa disciplina de Sistemas Embarcados a comunicação do ESP32 com a nuvem é a competência avaliada. **Registrar no relatório técnico como caminho de evolução**, para mostrar que a escolha foi consciente.
- **R15 Recuperação quando o AP não volta:** 2 tentativas (nunca mais, por causa do vazamento de R13b), depois `ESP.restart()`. O LED distingue três estados de rede: azul lento = sincronizando; azul rápido = vai reiniciar; azul fixo = de pé, aguardando login.
- **R16 `sessao_atual.json` é requisito, não conveniência.** Gravado antes de toda troca de rádio, lido no boot. Reinício no meio da aula retoma a sessão em curso (mesmo `professorId`, mesmo roster, janela ainda aberta) em vez de cair no login com presenças órfãs. É o que torna R15 aceitável.
- **R17 Repo público → `secrets.h` é obrigação, não boa prática.** Credenciais em `include/secrets.h` (no `.gitignore`); `include/secrets.example.h` versionado com os campos em branco. Chave que subir num repo público está comprometida para sempre, mesmo apagando o commit.

### Decisões operacionais

- **Bluetooth não será usado** — substituído pelo Wi-Fi nativo do ESP32 (o enunciado da Entrega 02 pedia Bluetooth por boilerplate de Arduino).
- **Aparelho fixo na sala** (decisão do professor + grupo) → alimentação por fonte 5V, caixa de parede, contas de vários professores no mesmo aparelho.
- **Cronograma válido = o revisado** (datas de terça: 07/09, 14/09, 22/09, 29/09, 05/10, 20/10, 27/10, 30/11). As datas de segunda que circulam são da outra turma.

## 3. Arquitetura em blocos

```
[Tag NFC 13,56 MHz] --RF--> [RC522] --SPI VSPI--> [ESP32 DevKit V1] <--GPIO34-- [SW-420 vibracao]
                                                        |
                                                        |--GPIO 25/26/27--> [LED RGB]
                                                        |--GPIO 33-------->  [Buzzer ativo]
                                                        |--LittleFS------->  professores.json / sessao_atual.json / eventos.json
                                                        |
                                            +-----------+-----------+
                                     modo AP (aula)          modo STA (2 momentos)
                                            |                       |
                            [Celular do professor]        [Hotspot celular] --> [API Supabase]
                             SoftAP WPA2 + portal                                  |
                             192.168.4.1 / DNS:53 / HTTP:80                  [PostgreSQL]
                                                                     (encena o sistema academico da UVA)
```

**O papel duplo da API no MVP:** como não há acesso ao sistema real da UVA, o Supabase **encena** esse sistema — é dele que vem a lista de alunos e é para ele que vai o relatório. Além disso serve de painel em nuvem para mostrar os relatórios acumulados na apresentação. Não é acessório: é peça obrigatória da narrativa.

## 4. Componentes

| Componente | Qtd | Função | Situação |
|---|---|---|---|
| ESP32 DevKit V1 / 32D | 1 | Controlador principal, Wi-Fi AP/STA alternado, WebServer, LittleFS | ✅ adquirido |
| Leitor RFID/NFC RC522 13,56 MHz | 1 | **Sensor 1** — leitura do UID (SPI VSPI: SCK 18, MISO 19, MOSI 23, SS 5, RST 22) | ✅ adquirido |
| Sensor de vibração SW-420 | 1 | **Sensor 2** — anti-violação: impacto/remoção do aparelho (GPIO 34, entrada) | ⬜ comprar (~R$6) |
| Tags NFC 13,56 MHz (NTAG215 + cartões brancos do kit) | 3–5 | Crachás de teste — firmware só lê o UID | ✅ adquirido |
| LED RGB catodo comum + 3× resistor 220 Ω | 1 | **Atuador 1** — verde/vermelho/azul (GPIO 25/26/27) | ⬜ comprar |
| Buzzer ativo 5V | 1 | **Atuador 2** — bip 100 ms na confirmação (GPIO 33) | ⬜ comprar |
| Protoboard 830 pontos | 1 | Montagem | ✅ adquirido |
| Jumpers M-M e M-F | 1 kit | Conexões | ⬜ comprar |
| Fonte 5V 2A / cabo micro-USB | 1 | Alimentação (aparelho fixo, sempre na tomada) | ⬜ comprar |

⚠️ **Atenção elétrica:** RC522 é estritamente **3,3V** — 5V queima o módulo. SW-420 alimentado em 3,3V para a saída ser compatível com o GPIO. Buzzer ativo 5V não liga direto no GPIO — usar transistor 2N2222 ou módulo com driver. GPIO 34 é **input-only**, correto para sensor, mas não tem pull-up interno (o SW-420 tem saída própria do comparador, então está ok).

Sem o RTC, o barramento I2C some e o conflito de pino SDA 21 / SCL 22 vs RST 22 que existia na versão anterior **deixa de existir**.

## 5. Firmware (C++, Arduino Core sobre PlatformIO)

Build: **PlatformIO** no VS Code (`platformio.ini` fixa as versões de biblioteca para os 5). Libs: `MFRC522` · `DNSServer` · `WebServer` · `LittleFS` · `ArduinoJson` · `HTTPClient` + `WiFiClientSecure` · `mbedtls` (SHA-256). Credenciais em `secrets.h`, fora do git.

Módulos em `src/`:

| Arquivo | Responsabilidade |
|---|---|
| `config.h` | Pinos, SSID/senha do AP, tempos, URL da API |
| `storage.cpp` | LittleFS: professores, sessão atual, append de eventos |
| `rfid.cpp` | Leitura de UID 4/7 bytes, debounce 5 s |
| `clock.cpp` | Hora recebida do navegador no Iniciar (ou da API), offset sobre `millis()` |
| `portal.cpp` | Login, cookie de sessão, rotas HTTP |
| `net.cpp` | Alternância AP↔STA (sequência fixa: parar server+DNS → `softAPdisconnect(true)` → `WIFI_OFF` → `delay(500)` → `WIFI_STA`; e o inverso na volta), roster, upload, retry **com limite** |
| `tamper.cpp` | SW-420: leitura, debounce, limiar, geração de alerta |
| `feedback.cpp` | LED + buzzer por máquina de estados com `millis()` — **sem `delay()`** |

Regras de performance: loop sem `delay()`; SPI em VSPI por hardware; `ArduinoJson` com documento estático (sem concatenar `String`); log serial com níveis.

### Máquina de estados

| Estado | LED | Som | Gatilho |
|---|---|---|---|
| `AGUARDANDO_LOGIN` | azul fixo | — | boot / sessão encerrada |
| `SINCRONIZANDO` | azul lento | — | rádio em STA (após o login, ou no Enviar) |
| `FALHA_DE_RADIO` | azul rápido | — | AP não voltou em 2 tentativas → `ESP.restart()` |
| `SESSAO_ABERTA` | verde fixo | — | professor clicou Iniciar |
| `REGISTRADO` | verde 2 piscadas | bip 100 ms | UID lido e gravado |
| `DUPLICADO` | vermelho 300 ms | — | mesmo UID dentro de 5 s — nada é gravado |
| `NAO_RECONHECIDO` | vermelho 1 s | buzz 400 ms | **só em modo online**: UID fora da lista da turma — nada é gravado |
| `ALERTA_VIOLACAO` | vermelho piscando | buzz longo | SW-420 acima do limiar — grava alerta, **não** interrompe a chamada |

Em **modo offline** o estado `NAO_RECONHECIDO` não existe: sem lista da turma, todo UID lido é aceito e vira `REGISTRADO`. O LED verde passa a significar *"li a tag"*, não *"você está nesta turma"*.

## 6. Portal do professor — fluxo canônico (16 passos)

HTML/CSS/JS vivem como arquivos no LittleFS (imagem de filesystem separada do firmware), servidos pelo `WebServer`. Ajuste de layout não exige recompilar o firmware.

1. Professor liga o aparelho
2. Aparelho sobe; professor procura as redes Wi-Fi
3. Professor conecta na rede do aparelho (senha WPA2)
4. Portal cativo abre a página de login automaticamente
5. Professor entra com login e senha. Validada a senha, o aparelho responde *"carregando turma…"* e **aproveita esse instante para sincronizar** (R14): rádio vai a STA, busca o roster, volta a AP. 5–15 s; o celular reconecta sozinho e o LED pisca azul lento
6. Painel abre **com a lista já em cache**. Seis ações: **Iniciar** · **Adicionar presença manual** · **Exibir relatório** · **Compartilhar relatório** · **Enviar relatório** · **Encerrar**
7. Professor clica Iniciar — **instantâneo**, sem rede
8. Sessão aberta com `professorId` gravado, LED verde fixo
9. Aluno encosta a tag
10. Aparelho lê o UID e resolve **localmente** contra a lista cacheada
11. Sem lista (modo offline): grava só o UID; o enriquecimento acontece no envio
12. Com lista: grava UID + matrícula + nome + horário
13. Professor pode clicar Exibir relatório a qualquer momento
14. No fim, professor clica Enviar. Se falhar, **Compartilhar** baixa o JSON no celular dele
15. Professor clica Encerrar
16. Aparelho volta ao passo 2/3 — página de login, pronto para o próximo professor

**LGPD por desenho:** o aparelho nunca guarda histórico de todos os alunos da instituição. Só a lista da turma da sessão corrente, apagada no Encerrar.

## 7. Contrato de dados (JSON)

```jsonc
// sessao
{ "sessaoId": "...", "professorId": "...", "disciplina": "...", "turma": "...",
  "inicio": "...", "fim": "...", "modo": "online" | "offline",
  "relogio": "portal" | "api" }

// evento
{ "uid": "A391F21B", "matricula": "...", "nome": "...",
  "timestamp": "ISO-8601 -03:00", "origem": "nfc" | "manual",
  "enriquecido": true | false, "lancado_por": "...", "motivo": "..." }

// alerta (SW-420)
{ "tipo": "violacao", "timestamp": "...", "duracao_ms": 0 }
```

Exportação:

```json
{
  "sessao": { "sessaoId": "20260907-T1", "professorId": "prof02",
              "disciplina": "Sistemas Embarcados", "turma": "3a-terca",
              "inicio": "2026-09-07T19:00:00-03:00", "modo": "online",
              "relogio": "portal" },
  "tempos": ["t1", "t2"],
  "roster_carregado": true,
  "eventos": [
    { "uid": "A391F21B", "matricula": "1240205596", "nome": "Caio P. O. Planinschek",
      "timestamp": "2026-09-07T19:05:22-03:00", "origem": "nfc", "enriquecido": true },
    { "matricula": "1240109764", "timestamp": "2026-09-07T19:06:10-03:00",
      "origem": "manual", "lancado_por": "prof02", "motivo": "esqueceu a tag" }
  ],
  "presentes": ["1240205596", "1240109764"],
  "faltantes": ["1240108001"],
  "alertas": []
}
```

`presentes` e `faltantes` só existem quando `roster_carregado: true`. Em modo offline o relatório sai só com `eventos`, e as listas são calculadas do lado da API no momento do envio.

**Supabase:** tabelas `alunos`, `sessoes`, `eventos`, `alertas`. Chave anon + RLS. O ESP32 faz `GET /alunos?turma=eq.<turma>` no Iniciar e `POST /sessoes` + `POST /eventos` no Enviar. Falha de rede → o relatório fica em `eventos.json` e o botão Compartilhar continua funcionando.

## 8. Plano de testes

| # | Teste | Critério |
|---|---|---|
| 1 | UID 4 bytes (Mifare), 3 aproximações seguidas | 1 evento só (debounce), <200 ms |
| 2 | UID 7 bytes (NTAG215) | registra pelo UID, sem escrita na tag |
| 3 | Modo online, tag fora da turma | vermelho longo, nada gravado |
| 4 | Modo offline (Iniciar sem internet) | `modo: "offline"`, aceita todo UID, relatório íntegro |
| 5 | Enviar sem internet → depois com internet | Compartilhar funciona; retry sobe o mesmo relatório sem duplicar |
| 6 | 2 professores em sequência | 2 sessões, `professorId` distintos, sem mistura |
| 7 | Alternância AP↔STA, 10 ciclos seguidos | AP volta nas 10 vezes; celular reconecta; transição dentro de 5–15 s |
| 7b | Falha forçada na volta ao AP (hotspot desligado no meio) | aparelho se recupera sozinho e **não perde** as presenças já registradas |
| 8 | SW-420: bater no aparelho durante a sessão | alerta gravado, chamada **não** interrompida, sem falso-positivo com porta batendo |
| 9 | Stress: 30 toques + reboot no meio | zero duplicata, zero perda (LittleFS persiste) |

## 9. Divisão da equipe

| Papel | Responsável | Entregável até 21–22/09 |
|---|---|---|
| Hardware | Gabriel Santarello | Protoboard montada, pinagem conferida, SW-420 calibrado, alimentação estável |
| Firmware núcleo | Cauã Andrade | `rfid.cpp` + `storage.cpp` + `feedback.cpp` + máquina de estados |
| Portal web | Igor Lobato | SPA do professor: login, painel de 6 botões, polling do relatório |
| Integração e documentação | Caio Planinschek | Rede (AP↔STA), Supabase, roster + upload; consolidação dos documentos de entrega |
| Estrutura física e diário de bordo | João Victor Cabral | Caixa de parede, diário de bordo semanal, fotos e vídeo |

Cada papel tem 1 dono, mas as fronteiras são permeáveis: quem terminar a sua frente ajuda a próxima. O papel de **Integração** existe justamente para circular entre as camadas e fechar o que ficar entre duas cadeiras.

**Fluxo de trabalho no repo:** commits direto na `main`, sem branch nem pull request. A modularização do firmware (§5) é o que evita conflito — cada frente no seu arquivo. Regra única: `git pull` antes de começar.

## 10. Principal desafio técnico

**Alternar o rádio único do ESP32 entre hotspot e cliente sem derrubar o portal do professor, mantendo o registro de presença abaixo de 200 ms e o relatório íntegro quando a troca falhar.**

Quatro coisas disputam o mesmo chip: o SPI do RC522 com UIDs de dois tamanhos, o WebServer + DNS do portal cativo, a escrita append-only no LittleFS (que não pode corromper em queda de energia) e a janela de STA para falar com a API.

A pesquisa técnica confirmou o desafio e o tornou mais preciso. O ESP32 tem **uma antena só**: rodar hotspot e cliente ao mesmo tempo faz o hotspot mudar de canal atrás da rede externa e derruba o celular do professor. Alternar entre os modos resolve o canal, mas leva **5 a 15 segundos** por transição e esbarra em bugs conhecidos do arduino-esp32 em que o hotspot simplesmente não volta. Some-se a isso o handshake TLS, que sozinho exige 40–50 KB de heap livre — o que torna **obrigatório** fechar o hotspot antes de abrir a conexão segura, por memória e não só por rádio.

Concentrar toda a rede em dois instantes (Iniciar e Enviar), em vez de uma chamada por aluno, é o que torna o problema tratável: reduz de ~40 janelas de risco por aula para 2. O preço é um modo offline degradado e uma recuperação de falha que precisam se comportar direito — porque quando a troca falha, o que está em jogo é uma aula inteira de presenças dentro do aparelho.

## 11. Glossário

**Tag** = crachá físico. **UID** = número de série do chip (4 ou 7 bytes). **Roster** = lista da turma (matrícula + nome + UID) baixada da API no Iniciar. **Sessão** = janela aberta pelo professor. **Evento** = 1 registro (nfc ou manual). **Modo offline** = sessão iniciada sem internet; grava só UIDs. **Enriquecimento** = juntar nome/matrícula ao UID. **Debounce** = ignorar repetição do mesmo UID por 5 s. **AP** = o aparelho como rede Wi-Fi. **STA** = o aparelho como cliente de outra rede.

## 12. Referências

OLIVEIRA & ANDRADE (2010) — hardware/firmware, barramentos SPI, temporização, drivers. GUIMARÃES (2009) — sensores e atuadores em sistemas embarcados. YAGHMOUR et al. (2009) — arquitetura de sistemas embarcados e integração (uso conceitual; o firmware aqui é bare-metal Arduino Core, não Linux embarcado).
