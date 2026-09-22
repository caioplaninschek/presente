# Presente! — SPEC

> Projeto: Terminal embarcado para contabilização de presença em sala de aula
> Disciplina: Sistemas Embarcados — UVA Barra, 3ª terça-feira · Turma **4172CMPN6A_P1** · Professor Thiago Alberto Ramos Gabriel
> Equipe: Gabriel Albuquerque Varela Santarello (1240110815) · Cauã Manuel Proença de Andrade (1240109764) · Igor Rocha Lobato (1240114118) · Caio Parada Oliveira Planinschek (1240205596) · João Victor Berçot Chabudet Cabral (1240108001)
> Status: catorze rodadas de decisão travadas — Q1–Q9 e R1–R8 em 05/09/2026, R9–R12 e R13 em 06/09/2026 00h, R14–R17 em 06/09/2026 01h, **R18 em 06/09/2026 manhã** (um sensor só) e **R19–R22 em 10/09/2026 noite** (primeira rodada decidida nas issues do GitHub: AP+STA reabre R4, modelo de dados, dono do dashboard, material completo) e **R23–R27 em 11/09/2026 madrugada** (a execução corrige a spec: a placa não circula, veredito parcial no 7c, credencial fora da URL, leitura do heap, onde os arquivos moram) e **R28–R29 em 12/09/2026** (a primeira linha de firmware: um ambiente por programa de bancada, versões fixadas) e **R30–R32 em 13/09/2026** (depois da Entrega 04: uma presença por sessão, o teste de memória suspeito resolvido na #18, o veredito parcial) e **R33–R35 em 13/09/2026** (o banco na nuvem: o aparelho só entra por duas funções, o histórico só com login, a chave publicável) e **R36–R41 em 15/09/2026** (a conferência das entregas do time: o evento é uma linha no disco, o horário do arquivo é o tempo desde o boot, a medição dos 200 ms é piso, o pull request como exceção, nenhuma tela avança sem confirmação, a partição única) e **R42–R43 em 16/09/2026** (o pente-fino do repositório: o aparelho só escreve `finalizada`, e o que fica na saída não vira coluna) e **R44 em 21/09/2026** (a checagem do buzzer termina no transistor, e a peça atual precisa ser trocada). **Entrega 03 entregue em 07/09/2026**, transcrita em `entregas/entrega-03/entrega-03.md`. ⚠️ O documento entregue é um **subconjunto comprimido** desta spec: coube em quatro páginas cortando detalhe de quase toda seção. **Esta spec é o registro completo** — nada do que saiu do PDF saiu daqui. **Entrega 04 entregue** (prazo em 14/09, 23:59), transcrita em `entregas/entrega-04/entrega-04.md`, também um subconjunto comprimido. Próxima base: **Entrega 05** — integração sensor + atuador com testes documentados. O enunciado saiu em 20/09: envio no Teams até **22/09, 23:59**, e demonstração presencial obrigatória na aula do mesmo dia (`docs/entregas.md`). Spec incrementada em 08/09/2026 com histórias de usuário (§1.1), ligações do ESP32 (§4.1), pseudocódigo (§5) e política de teste (§8), em 10/09/2026 com a rodada 7 (§2), os testes 7c/7d (§8) e o desafio técnico reescrito (§10), em 11/09/2026 com a rodada 8 (§2), as lacunas conhecidas e os critérios corrigidos de 7c/7d (§8) e a divergência declarada da divisão de equipe (§9), e em 13/09/2026 com a rodada 10 (§2), duas lacunas novas (§8) e os termos de registro e presença no glossário (§11), e ainda em 13/09/2026 com a rodada 11 (§2), o banco no §7, uma lacuna nova (§8) e a chave publicável no glossário (§11), e em 15/09/2026 com a rodada 12 (§2), as rotas provisórias do protótipo no §6, o arquivo de eventos no §7, o critério do teste 1, o gatilho do `loop()` e três lacunas novas no §8, e a exceção do pull request no §9, e em 16/09/2026 com a rodada 13 (§2), o que o aparelho escreve no banco no §7 e os termos portal do professor, painel e portal cativo no glossário (§11), e em 21/09/2026 com a rodada 14 (§2), o LED e o buzzer como foram montados no §4 e no §4.1, e a primeira medição de memória no R19.

## 1. Problema e objetivo

Professores da UVA Barra gastam 10–15 min por aula fazendo chamada manual no site da UVA: tabela com 2 checkboxes por aula (marcadas = falta, desmarcadas = presença), leitura de nome por nome, pergunta "está presente?", marcação manual. Gera perda de tempo de aula, erro de linha (marcar falta no aluno errado), fraude potencial e esforço vocal.

Objetivo do protótipo: terminal embarcado **fixo ao lado da porta** que registra presença por aproximação de tag NFC, com feedback imediato audiovisual, gestão pelo professor via Wi-Fi próprio do aparelho e relatório JSON que sobe para uma API na nuvem — com exportação local garantida quando não houver internet.

**Fora do escopo do protótipo.** Cada corte abaixo foi decidido, não esquecido — a razão de cada um vive aqui. O `map.md` do Wayfinder repete a lista em uma linha por item, como índice.

- **Integração real com o sistema acadêmico da UVA** — não há acesso ao sistema. O Supabase **encena** esse papel no MVP (§3).
- **Celular do aluno como tag** (NFC no aparelho pessoal) — citado na documentação como caminho de evolução, não construído.
- **Biometria e catraca** — custo, LGPD (biometria é dado sensível) e escopo.
- **Controle de saída da sala** — R8: duplica estado, duplica falha e não resolve o problema que motivou o projeto. Diverge do Canvas da Entrega 02; a divergência foi declarada na Entrega 03.
- **Backend próprio auto-hospedado** (Node/Python + PostgreSQL) — substituído pelo Supabase.
- **Segundo sensor e detecção de violação** (SW-420) — R18: o professor dispensou a exigência de dois sensores e o sensor de vibração saiu do projeto inteiro.

### 1.1 Histórias de usuário

O comportamento esperado do sistema pelo ponto de vista de quem usa. O §6 diz *em que ordem* as coisas acontecem; esta seção diz *por que cada uma precisa acontecer*, e é a referência citável (`HU-xx`) para os critérios de aceitação dos tickets de implementação. As duas histórias que dependiam de decisão aberta (`HU-30` e `HU-39`) foram fechadas em R20 e R21.

#### Professor — o usuário do aparelho

1. **HU-01** Como professor, quero encontrar a rede do aparelho na lista de Wi-Fi do meu celular, para começar a chamada sem instalar nada.
2. **HU-02** Como professor, quero que a página de login abra sozinha quando eu conectar, para não precisar decorar endereço de IP.
3. **HU-03** Como professor, quero entrar com um login e uma senha só meus, para que a chamada que eu abrir fique registrada no meu nome.
4. **HU-04** Como professor, quero trocar a senha padrão no primeiro uso, para que minha conta não fique com a senha de fábrica.
5. **HU-05** Como professor, quero que o aparelho baixe a lista da turma enquanto eu faço login, para que o **Iniciar** seja instantâneo e a espera caia no instante em que eu já espero que a página demore.
6. **HU-06** Como professor, quero ver pelo LED e pela tela que o aparelho está sincronizando, para não achar que travou enquanto ele sincroniza, o que no Plano B inclui os 5–15 s da troca de rádio.
7. **HU-07** Como professor, quero abrir a chamada com um clique, para não gastar tempo de aula com configuração.
8. **HU-08** Como professor, quero que uma chamada cubra os dois tempos da aula, para não repetir o processo no segundo tempo.
9. **HU-09** Como professor, quero lançar presença pela matrícula, para resolver na hora o aluno que esqueceu a tag.
10. **HU-10** Como professor, quero que o lançamento manual fique marcado como manual, com quem lançou e por quê, para que a chamada continue auditável.
11. **HU-11** Como professor, quero ver presentes e faltantes a qualquer momento durante a aula, para conferir antes de encerrar.
12. **HU-12** Como professor, quero enviar o relatório para a nuvem no fim da aula, para que a presença fique registrada fora do aparelho.
13. **HU-13** Como professor, quero baixar o relatório no meu celular quando o envio falhar, para não perder uma aula inteira de presenças por causa de internet.
14. **HU-14** Como professor, quero encerrar a sessão e devolver o aparelho ao login, para que o próximo professor use o mesmo aparelho sem ver os meus dados.
15. **HU-15** Como professor, quero configurar o SSID e a senha do hotspot pelo próprio portal, para trocar de celular sem reprogramar o aparelho.
16. **HU-16** Como professor, quero que o aparelho não guarde histórico de aluno depois da aula, para que uma caixa presa na parede não vire um banco de dados sem dono.

#### Aluno — quem encosta a tag

17. **HU-17** Como aluno, quero registrar presença encostando a tag, para não perder aula esperando a chamada nominal.
18. **HU-18** Como aluno, quero um sinal imediato — verde e bip — de que fui registrado, para saber que posso sentar sem perguntar.
19. **HU-19** Como aluno, quero que encostar duas vezes não me registre duas vezes, para não gerar dúvida no relatório.
20. **HU-20** Como aluno, quero um sinal diferente quando a tag não é reconhecida, para procurar o professor em vez de sentar achando que está tudo certo.
21. **HU-21** Como aluno, quero receber a tag já cadastrada, para não depender de ninguém me cadastrar no aparelho.
22. **HU-22** Como aluno, quero que minha tag seja apenas lida, nunca escrita, para poder usar o mesmo crachá em outros lugares.

#### Equipe — quem constrói e opera o protótipo

23. **HU-23** Como integrante da equipe, quero as versões das bibliotecas fixadas no `platformio.ini`, para que o firmware compile igual nas cinco máquinas.
24. **HU-24** Como integrante da equipe, quero editar o HTML do portal sem recompilar o firmware, para que portal e firmware avancem em paralelo.
25. **HU-25** Como integrante da equipe, quero cada frente no seu próprio arquivo, para commitar direto na `main` sem conflito.
26. **HU-26** Como integrante da equipe, quero as credenciais fora do git, para que a chave do Supabase não vaze num repositório público.
27. **HU-27** Como integrante da equipe, quero um diário de bordo semanal legível, para provar evolução ao professor sem que ele precise ler o histórico do git.
28. **HU-28** Como integrante da equipe, quero um checklist de hotspot impresso, para que o pareamento não falhe na apresentação.
29. **HU-29** Como integrante da equipe, quero um SSID de reserva já gravado no aparelho, para não depender de um único celular no dia.
30. **HU-30** Como integrante da equipe, quero mostrar o histórico acumulado de várias aulas, para que a apresentação mostre um sistema e não um leitor de crachá. *(fechada em R21: a página existe e é do Igor — issue #2, opção B)*

#### Falha e degradação — o que sustenta a aula quando algo dá errado

31. **HU-31** Como professor sem internet no login, quero que a chamada aconteça mesmo assim, para não perder a aula por causa do hotspot.
32. **HU-32** Como professor em modo offline, quero que o relatório seja completado no envio, para não digitar nome nenhum depois.
33. **HU-33** Como professor em modo offline, quero que o relatório declare que a sessão foi offline, para que quem o receber saiba por que faltam os nomes.
34. **HU-34** Como professor em modo offline, quero saber que o aparelho aceita **qualquer** tag lida — sem a lista, ele não sabe quem é da turma —, para conferir a lista depois em vez de confiar no verde.
35. **HU-35** Como professor, quero que uma queda de energia no meio da aula não apague as presenças já registradas, para não refazer a chamada.
36. **HU-36** Como professor, quero que o aparelho retome a sessão em curso depois de reiniciar, para não cair no login com presenças órfãs.
37. **HU-37** Como professor, quero que o aparelho se recupere sozinho quando o hotspot não voltar, para não ter que mexer nele durante a aula.
38. **HU-38** Como integrante da equipe, quero que falhas de conexão segura não travem o aparelho, para que ele não precise de reset físico na sala.
39. **HU-39** Como professor, quero que clicar **Enviar** duas vezes não duplique o relatório, para não sujar o registro. *(fechada em R20: sessão `finalizada` recusa o segundo `POST`)*
40. **HU-40** Como professor, quero que o registro continue abaixo de 200 ms com o aparelho cheio de eventos, para não formar fila na porta.

## 2. Decisões travadas

### Rodada 1 — Q1–Q9 (05/09, madrugada)

- **Q2 Tags:** firmware só **LÊ** o UID, nunca escreve na tag. Mifare (4 bytes) e NTAG215 (7 bytes) funcionam igual. Trabalhar com as tags já compradas; cartões brancos do kit RC522 servem de teste extra.
- **Q4 Auth:** até ~10 contas locais em `professores.json`, senha como hash SHA-256 + salt, sessão por cookie. Cada sessão carrega `professorId`. Troca de senha default no primeiro boot.
- **Q5 Semântica:** 1 toque = presente. Ausência = falta. Debounce: mesmo UID ignorado por 5 s. Meta de latência: tag → LED+buzzer em **<200 ms**. ⚠️ **Corrigido em R30 (13/09):** o UID já registrado não se registra de novo em nenhum momento da sessão; os 5 s passam a ser só a janela de silêncio para o crachá parado no leitor.
- **Q6 Fallback:** professor lança matrícula manual pelo portal (`origem: "manual"`, com `lancado_por` + `motivo`).
- **Q8 Tempos UVA:** 1 sessão de chamada cobre os 2 tempos (`tempos: ["t1","t2"]`, 1 toque = presente nos dois).

### Rodada 2 — R1–R8 (05/09, 12h) — **revisa a rodada 1**

- **R1 Cadastro de tag sai do aparelho.** A tag chega ao aluno **já cadastrada** — pela equipe no protótipo, pela instituição no cenário real. O professor nunca cadastra ninguém. Consequência: **`turma_ativa.json` deixa de existir**; a lista da turma passa a vir da API.
- **R2 Sincronização em 2 momentos, nunca por toque.** ❌ Descartado: uma chamada de API por aluno (40 alunos = 40 idas à internet durante a aula, latência refém do Wi-Fi, fila na porta). ✅ Adotado:
  - **Iniciar** → 1 chamada: baixa a lista da turma (matrícula + nome + UID, ~3 KB p/ 40 alunos) e cacheia no LittleFS pela sessão. ⚠️ **Corrigido em R14 (06/09):** essa chamada passou para o login. ⚠️ **Medido em 15/09, na #21 (R33):** a lista da turma de 40 alunos dos dados de teste veio com 3230 bytes.
  - **Durante a aula** → zero internet. Consulta local, <200 ms garantido.
  - **Enviar** → 1 chamada: sobe o relatório completo.
- **R3 Modo offline degradado.** Sem internet no Iniciar, o aparelho entra em **modo UID-cru**: registra só os números das tags, marca `modo: "offline"`, e o enriquecimento (nome/matrícula) acontece no envio. Nada se perde. Consequência: **offline o aparelho não sabe validar turma** — aceita qualquer UID lido, e o backend julga depois. ⚠️ **Corrigido em R14 (06/09):** o modo offline se decide no login, quando a sincronização falha, e não no Iniciar.
- **R4 AP e STA nunca simultâneos.** O ESP32 tem uma antena só; ser hotspot e cliente ao mesmo tempo derruba o portal do professor (o softAP muda de canal para acompanhar o STA). Como só existem 2 momentos de internet, o rádio **alterna**: `AP → STA → AP`. ⚠️ **Corrigido em R13:** a transição leva **5–15 s**, não os ~3 s estimados aqui. ⚠️ **Reaberto em R19 (10/09):** a premissa "nunca simultâneos" está **errada**. R4 deixa de ser decisão travada e passa a ser o **Plano B** — a alternativa, caso o teste reprove o modo simultâneo.
- **R5 Internet vem de hotspot de celular**, com SSID e senha configuráveis pelo próprio portal. ⚠️ O ESP32 **não** passa por portal cativo nem autenticação corporativa, então a rede da UVA **está fora** — confirmado em 10/09 na issue #3, com dois motivos observados além do portal: a rede nega login a aluno matriculado (o Caio não entra desde 2026.1) e o filtro de saída bloqueia serviço legítimo (Google Drive), comportamento que não dá para prever do lado do aparelho. Documentar: "em produção a instituição provisionaria rede dedicada aos dispositivos".
- **R6 Duas camadas de senha, não uma.**
  - **WPA2 no SoftAP** (mesma para todos, não é segredo, pode estar colada na caixa) → criptografa o ar. Sem ela a senha do professor trafega legível na sala.
  - **Login individual por professor** (hash local) → identidade. Continua um por professor, como na Q4.
- **R7 Sensor 2 = SW-420 (vibração) como anti-violação.** ❌ Descartado o **RTC DS3231**: é redundante (o navegador do professor entrega a hora no Iniciar; a API também) e discutível como "sensor". ❌ Descartado sensor de porta (PIR/ultrassônico): outro grupo da turma faz projeto similar com sensor de porta. ✅ SW-420: o aparelho é **fixo, sozinho na sala e guarda dados de presença** — um aluno tem motivo para mexer nele. Impacto/violação vira alerta no relatório. ⚠️ Módulo é ruidoso: exige debounce em software **e** calibração do potenciômetro de limiar já montado no lugar definitivo. ⚠️ **Revertido em R18:** o professor dispensou o segundo sensor e o SW-420 saiu do projeto.
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
  - **Custo assumido:** `setInsecure()` criptografa mas **não autentica o servidor** — MITM teoricamente possível no hotspot. Proporcional (hotspot do próprio grupo, chave publicável e descartável — R35 —, protótipo acadêmico). **Registrar essa limitação no relatório técnico**, não escondê-la.
  - **Sem a lib `ESPSupabase`:** parada desde 07/2025, arrasta `WebSockets` como dependência morta, e internamente só faz o que 20 linhas próprias fazem.
- **R13a Fechar o AP antes de abrir o TLS é requisito de memória.** O handshake pede 40–50 KB de heap livre (mbedTLS aloca 16 KB RX + 16 KB TX). R4 deixa de ser só questão de canal de rádio. ⚠️ **Com R19, este vira o argumento decisivo:** o canal deixou de ser impedimento para o modo simultâneo, a memória não. O Plano A só passa se sobrarem os 40 KB com AP, DNS, WebServer e roster de pé ao mesmo tempo — é medição, não estimativa.
- **R13b Limitar as tentativas de `connect()`.** `WiFiClientSecure` vaza ~4 KB de heap por conexão **falha** (arduino-esp32 #3808); retry sem limite trava o aparelho.
- **R13c `getStream()` no GET do roster**, nunca `getString()` — `getString()` em resposta de ~3 KB fragmenta o heap.
- **R13d Checklist de hotspot** (vai impresso para a apresentação):
  - **iPhone:** ligar *"Maximizar Compatibilidade"* no Ponto de Acesso (força 2,4 GHz); manter a tela do hotspot aberta durante o pareamento (o iOS suspende o beacon); nome do iPhone sem acento nem caractere especial, porque vira o SSID.
  - **Android:** escolher a banda 2,4 GHz explicitamente; senha sem caractere especial.
  - Gravar um **SSID de fallback** no aparelho antes do dia da apresentação.
  - Abrir o painel do Supabase antes do dia e, se o projeto estiver pausado, retomá-lo (Resume project). Projeto gratuito pausa depois de 7 dias com pouco uso, e com o projeto pausado o login do aparelho cai no modo offline sem aviso (R33, 13/09).

### Rodada 5 — R14–R17 (06/09, 01h)

- **R14 A sincronização acontece no login, não no Iniciar.** O aparelho valida a senha, responde *"carregando turma…"*, e só então troca o rádio. Quando o painel abre, o roster está em cache e o **Iniciar** é instantâneo. A espera de 5–15 s não some — muda para o instante em que o professor já espera que a página demore. Altera o §6.
  - Alternativa considerada e **descartada por razão acadêmica**: tirar a internet do ESP32 e deixar o celular do professor falar com o Supabase (o aparelho nunca sairia do modo AP). Mataria a troca de rádio, o bug do AP, o TLS e 40–50 KB de heap — mas numa disciplina de Sistemas Embarcados a comunicação do ESP32 com a nuvem é a competência avaliada. **Registrar no relatório técnico como caminho de evolução**, para mostrar que a escolha foi consciente.
- **R15 Recuperação quando o AP não volta:** 2 tentativas (nunca mais, por causa do vazamento de R13b), depois `ESP.restart()`. O LED distingue três estados de rede: azul lento = sincronizando; azul rápido = vai reiniciar; azul fixo = de pé, aguardando login.
- **R16 `sessao_atual.json` é requisito, não conveniência.** Gravado antes de toda troca de rádio, lido no boot. Reinício no meio da aula retoma a sessão em curso (mesmo `professorId`, mesmo roster, janela ainda aberta) em vez de cair no login com presenças órfãs. É o que torna R15 aceitável.
- **R17 Repo público → `secrets.h` é obrigação, não boa prática.** Credenciais em `include/secrets.h` (no `.gitignore`); `include/secrets.example.h` versionado com os campos em branco. Chave que subir num repo público está comprometida para sempre, mesmo apagando o commit.

### Rodada 6 — R18 (06/09, 09h30) — dispensa do segundo sensor

- **R18 O projeto passa a ter um sensor só, o RC522.** O João confirmou com o professor que a exigência de dois sensores está dispensada para este projeto. **Reverte o R7:** o SW-420 sai. Motivo para tirar em vez de manter por segurança: o sensor de vibração não participa da chamada — é uma frente paralela (compra, calibração de potenciômetro no lugar definitivo, debounce próprio, um estado a mais na máquina de estados e um tipo a mais no contrato de dados) fora do problema que motivou o projeto. Saem junto o `tamper.cpp`, o estado `ALERTA_VIOLACAO`, o alerta no contrato de dados, a tabela `alertas` no Supabase e o teste 8, que era o de violação (o de estresse, então teste 9, passou a ser o 8). ⚠️ A exigência escrita ("mínimo de 2 sensores", em `docs/entregas.md`) continua publicada no Teams: a Entrega 03 declara a dispensa em uma linha, para não ser lida como descumprimento.

### Rodada 7 — R19–R22 (10/09, noite) — respostas do grupo nas issues

Primeira rodada decidida **fora do chat**: as quatro decisões abaixo saíram das issues #1 a #4 do GitHub, com o grupo por escrito e datado.

- **R19 O modo `WIFI_AP_STA` existe e é nativo; a escolha entre simultâneo e alternado vira medição.** O João trouxe a correção na issue #3 e ela procede — **R4 estava errado na premissa**. A documentação da Espressif (guia de Wi-Fi do ESP-IDF) diz que os dois modos coexistem, com uma restrição: `"In Station/AP-coexistence mode, the home channel of AP and station must be the same, if they are different, the station's home channel is always in priority."` O softAP acompanha o canal do STA e anuncia a troca por **CSA**; estação que suporta channel switch acompanha **sem cair**. Ou seja, o canal para de ser impedimento — o que sobra como risco é a memória (R13a). O projeto passa a ter dois planos:
  - **Plano A — `WIFI_AP_STA`.** AP e STA no ar ao mesmo tempo; o professor nunca perde a conexão. Mata a transição de 5–15 s, o bug do AP que não volta (R15) e o `sessao_atual.json` como salva-vidas de reboot (R16 continua válido por queda de energia).
  - **Plano B — alternância `AP → STA → AP`.** O que R4 especifica hoje. Só entra se o A reprovar.
  - **Critério de decisão, medido na bancada (testes 7c e 7d do §8):** o celular do professor sobrevive ao pulo de canal, em iPhone **e** Android; e sobram ≥40 KB de heap no instante do handshake, com AP + DNS + WebServer + roster carregados. Reprovou qualquer um dos dois → Plano B.
  - ⚠️ **Medido em 21/09, pelo João:** a memória passa com folga. Antes da conexão segura sobraram **236,5 KB** sem nenhum celular na rede do aparelho (#17, uma rodada) e **229,6 KB** com um celular conectado (#18), contra os 40 KB do critério. O critério do painel ainda não tem rodada válida: na única rodada da #18, o aparelho já estava conectado ao hotspot quando o botão foi apertado, e o pulo de canal não aconteceu. O veredito (#19) espera essa rodada.
  - ⚠️ **O datasheet do módulo não responde isso.** `docs/datasheets/esp32-wroom-32/esp32-wroom-32d_esp32-wroom-32u_datasheet_en.md` (v2.7) é documento de hardware: confirma antena única, 2,4 GHz e 520 KB de SRAM, e não menciona modo de Wi-Fi. Comportamento de rádio e de software vive em https://docs.espressif.com/projects/esp-idf/en/v4.4/esp32/api-guides/wifi.html — **usar essa fonte, não o datasheet, para pergunta de comportamento.**
- **R20 O modelo de dados ganha três entidades, por proposta do Cauã na issue #1.** Além dos seis pontos já sugeridos (aceitos pelo Igor e pelo Cauã, sem objeção do Gabriel até a publicação): **professor como cadastro próprio** (o login do painel precisa conferir a senha contra alguma coisa, e o relatório precisa dizer quem deu a aula; ⚠️ **corrigido em R33 (13/09):** a senha é conferida no aparelho, em `professores.json` (Q4), porque o login precisa funcionar sem internet, e a tabela `professores` do banco guarda só a identidade); **turma como tabela, com `alunos_turmas` no meio** (sem isso "turma" é texto solto dentro da sessão, e o mesmo aluno aparece grafado de três jeitos); e **sessão com `estado`** (`aberta` / `finalizada` / `cancelada`). O estado não é só conveniência de dashboard: **é ele que implementa a idempotência** do ponto 5 da pauta — sessão `finalizada` recusa o segundo `POST`, e `HU-39` deixa de estar em aberto.
- **R21 O dashboard em nuvem existe, e o dono é o Igor** (issue #2, opção B: página simples lendo o histórico do banco, sem tocar no firmware). Fecha `HU-30`. Não depende do firmware: a página lê o banco, o aparelho escreve no banco, e os dois nunca se falam direto. ⚠️ Registrar para o grupo, porque a dúvida apareceu duas vezes: **não há API para escrever.** O `PostgREST` do Supabase gera os endpoints a partir das tabelas; o trabalho real é criar as tabelas e escrever as regras de RLS, e é do Caio. ⚠️ **Completado em R33 (13/09):** o aparelho não usa os endpoints das tabelas. O trabalho real são as tabelas, os privilégios de cada papel e duas funções dentro do próprio banco, que o `PostgREST` expõe da mesma forma, e continua sem servidor para escrever.
- **R22 O material está completo e a caixa sai do caminho crítico** (issue #4). O João comprou tudo e as peças estão com ele — a lista do §4 fecha em ✅. A caixa não entra no protótipo de 22/09; o material só se escolhe depois da placa montada, quando o tamanho real for conhecido. Entrega da estrutura física continua em 29/09.
  - ⚠️ **Antes de comprar transistor para o buzzer, checar na bancada:** módulo de 3 pinos já traz o driver embutido; peça solta de 2 pinos pode bastar em 3,3 V, já que o bip precisa ser audível a um passo do aparelho, não alto. Só se as duas reprovarem entra NPN — e qualquer um serve (2N2222, BC337, S8050), em loja física, por um ou dois reais.

### Rodada 8 — R23–R27 (11/09, madrugada) — a execução corrige a spec

Primeira rodada que nasce da **execução**, não de decisão pendente. Ao quebrar o trabalho em 24 issues do GitHub e 19 tickets locais, cinco coisas que já valiam na prática apareceram sem registro aqui — uma delas contradizendo o que esta spec afirma. Esta rodada as traz para a fonte da verdade.

- **R23 A placa não circula: ela fica com o João e o trabalho chega até ela mastigado.** Decisão do Caio em 11/09. Há **um microcontrolador só**, e a divisão por papéis do §9 pressupõe quatro frentes tocando a placa em paralelo — uma fila que não existe recurso para atender. O modelo novo: toda task física nasce partida em **preparar** (qualquer um, sem placa) e **rodar** (João, com a placa na mesa dele), e o enunciado chega pronto — código, passo a passo, o que tem que aparecer, o que fazer se der errado. Consequência direta: **a tabela do §9 deixa de valer como contrato**, e a divergência está declarada ao pé dela. Custo assumido: nove execuções físicas em onze dias, todas pelo João; se empilhar, a saída é uma tarde na casa dele com mais gente, ou a placa circular por alguns dias.
- **R24 O teste 7c aceita veredito parcial.** O critério do §8 pede iPhone **e** Android, e o 7c é o único item do projeto que exige dois celulares no mesmo cômodo da placa. Se os dois que o João conseguir forem do mesmo sistema, ele roda assim mesmo e anota qual foi: o veredito sai **parcial**, e o outro lado fecha quando houver um celular do outro sistema perto da placa. Veredito parcial é melhor que veredito nenhum — o que não se aceita é o teste não acontecer. O critério do 7c no §8 foi corrigido para dizer isso; até aqui a issue #18 estava formalmente fora da spec.
- **R25 A credencial da sessão não viaja no endereço da página.** Objeção do Cauã, aceita pelo Caio na issue #3 em **10/09**: endereço com token dentro fica exposto em histórico de navegador, em captura de tela e em registro de acesso de servidor. A decisão estava no `DIARIO.md` e na issue, nunca aqui — é a mais séria das três divergências encontradas em 11/09, porque é decisão pública, com autor e data, que a fonte da verdade não tinha absorvido. **Derruba a opção (c) do item 2 do ticket `.scratch/presente/issues/09`**, que ainda a listava viva e em pé de igualdade com as outras. Restam (a) manter tudo no CNA e (b) trazer o professor pelo CNA e seguir em `192.168.4.1` no navegador normal — e a escolha entre as duas continua sendo medição de bancada, não conversa.
- **R26 Medição de heap com um celular só sai otimista.** No teste 7d o hotspot ocupa o único celular disponível, então **ninguém fica conectado ao AP durante a medição** — e cliente associado consome memória. O número medido é teto, não piso. Regra de leitura: resultado **entre 40 e 45 KB é suspeito**, não aprovado; medir de novo com um cliente conectado assim que houver um segundo aparelho.
- **R27 Onde os arquivos moram, nomeado.** O R10 e o §6 diziam "arquivos no LittleFS" sem apontar a pasta, e o §8 exigia evidência gravada sem dizer onde ela fica. Ficam fixados: portal do professor em **`data/`** (convenção do PlatformIO — é a pasta que vira a imagem de filesystem); evidência dos testes em **`docs/assets/testes/`**, consolidada ~~em **`docs/testes-22-09.md`**~~ na seção 3 do rascunho da entrega, em `docs/entregas/entrega-05/` *(atualizado em 22/09: o arquivo separado duplicaria a seção 3, que já era o mesmo documento)*; fluxograma da lógica em **`docs/fluxograma-logica.drawio`**, ao lado do `diagrama-blocos.drawio` do §3.

### Rodada 9 — R28–R29 (12/09) — a primeira linha de firmware decide onde o código mora e em que versão

Nasce da issue #14, o ambiente de compilação. Escrever o `platformio.ini` obrigou a responder duas perguntas que as issues seguintes pressupunham sem dizer: onde fica cada programa de bancada e em que versão o firmware compila.

- **R28 Cada programa de bancada mora na sua pasta, com o seu ambiente.** Programa novo ganha `src/<nome>/` e um `[env:<nome>]` no `platformio.ini`, que compila só aquela pasta. Nomes fixados de uma vez, sem acento, porque viram pasta e aparecem no seletor do VS Code: `ambiente` (#14), `radio` (#15, rodado de novo pela #17 e pela #18), `prova-de-vida` (#24), `leitor` (#25), `encostou` (#26), `persistencia` (#27). **Por quê:** cinco programas são preparados em paralelo, por pessoas diferentes, entre 15 e 19/09, e a #17 precisa rodar de novo o programa do rádio (R26) — tudo num arquivo só faria um apagar o outro, contra a HU-25. **Consequências para quem prepara:**
  - O passo a passo do João nomeia o ambiente a escolher na barra azul do VS Code antes do Upload. **`default_envs` nunca é definido:** com "Default" selecionado, o Upload grava todos os ambientes em sequência e a placa fica com o último, sem erro nenhum.
  - O João nunca edita arquivo versionado: edição local dele trava o `git pull` da tarefa seguinte. Ajuste volta como comentário na issue, e quem preparou commita.
  - Código que dois programas dividirem sobe para **`lib/<módulo>/`**, com o nome do módulo do §5 que ele vai virar (`rfid`, `feedback`, `storage`…). É o mecanismo nativo do PlatformIO: tudo em `lib/` fica visível a todos os ambientes, sem configuração.
  - **O que a R28 não decide:** o arranjo do firmware de verdade (módulos do §5), que se monta depois de 22/09.
  - ~~`src/main.cpp` único, com "o programa da vez"~~ — preparadores em paralelo se sobrescrevem, e o programa do rádio não poderia ser rodado de novo (12/09).
  - ~~`default_envs` trocado pelo preparador a cada task~~ — é uma linha compartilhada editada em paralelo, exatamente o conflito que esta decisão evita (12/09).
- **R29 As versões fixadas, e as duas alternativas derrubadas.** Plataforma **`espressif32 7.1.3`**, que traz o Arduino core **2.0.17 (IDF 4.4.7)** — a base sobre a qual R13 e R19 foram pesquisadas, e a feita para o PlatformIO Core 6.2, que uma instalação nova entrega desde 05/09. Placa `esp32doit-devkit-v1`. Bibliotecas **`MFRC522 1.4.12`** e **`ArduinoJson 6.21.6`**; as nativas do §5 (`WebServer`, `DNSServer`, `LittleFS`, `HTTPClient`, `WiFiClientSecure`, `mbedtls`) vêm fixadas pela plataforma. Nenhuma biblioteca entra sem versão. As versões vivem no `platformio.ini`; a spec guarda o porquê.
  - ~~Arduino core 3.x (fork pioarduino)~~ — troca o mbedTLS e a pegada de memória antes da medição de heap da #17 e invalida as referências de R13 e R19 (12/09).
  - ~~ArduinoJson 7~~ — não tem documento estático de verdade: o `StaticJsonDocument` virou alias que aloca no heap, compila e perde em silêncio a garantia que o §5 exige (12/09).

### Rodada 10 — R30–R32 (13/09) — depois da Entrega 04

Nasce da conferência do repositório inteiro contra as issues, feita depois da entrega. Uma decisão muda o que acontece quando o aluno encosta o crachá de novo; as outras duas dizem o que fazer quando os testes de rádio saírem incompletos. As três são do Caio.

- **R30 Uma presença por sessão.** O UID que já está registrado na sessão não se registra de novo, em momento nenhum. O aluno que encosta outra vez recebe `JA_REGISTRADO` — verde, uma piscada, sem bip — e nada é gravado. Até aqui só os primeiros 5 s estavam protegidos (Q5): um toque depois disso gravava um segundo evento, e a HU-19 prometia mais do que a máquina de estados fazia. **Consequências:**
  - Os 5 s continuam só como **janela de silêncio**: leituras do mesmo UID nesse intervalo são ignoradas sem sinal nenhum, para o crachá parado no leitor não repetir o feedback.
  - **A regra sobrevive ao reinício.** Ao retomar a sessão (R16), o aparelho relê o `eventos.json` para saber quem já está registrado. Sem isso, um reinício no meio da aula reabriria o registro duplo.
  - **Lançamento manual.** Com a lista da turma carregada, o portal recusa lançar quem já está presente, e o toque de quem foi lançado à mão dá `JA_REGISTRADO`. Sem a lista (modo offline), o aparelho não tem como ligar a matrícula ao UID: grava os dois eventos, e a API os junta no envio, como já faz com `presentes` e `faltantes` (§7).
  - O estado se chamava `DUPLICADO` e foi renomeado para não disputar sentido com **duplicata**, que no teste 8 é defeito (§11).
  - ~~O mesmo UID, passados os 5 s, grava evento novo~~ — a HU-19 promete ao aluno não ser registrado duas vezes, e o segundo evento só não virava presença dupla porque `presentes` é calculado (13/09).
  - ~~Repetição sinalizada com vermelho de 300 ms~~ — vermelho é o sinal do crachá não reconhecido (HU-20), e o aluno que já está na lista entenderia que deu erro (13/09).
- **R31 O teste 7d suspeito se resolve na #18.** Abaixo de 40 KB, a #17 reprova o Plano A, e a #18 não acontece. De 40 a 45 KB o resultado é suspeito (R26): a #18 acontece mesmo assim e, como nela o celular do professor fica conectado à rede do aparelho, é a memória medida ali que decide. Acima de 45 KB, a #18 confirma o número com cliente conectado. Nenhuma sessão de bancada a mais.
  - ~~Suspeito repete a #17 num dia com dois celulares~~ — a #18 já é esse dia (13/09).
  - ~~Suspeito conta como reprovado~~ — descartaria o Plano A por um número que a própria R26 diz ser teto, e não piso (13/09).
- **R32 O veredito parcial adota o Plano A provisoriamente.** Se o 7c sair parcial (R24) e aprovado, e a memória passar, a spec adota o Plano A para a integração e registra no R19 qual sistema de celular faltou. Esse sistema é testado até a entrega do dashboard (05/10 pelo cronograma de 05/09): se reprovar, o projeto vai para o Plano B, que continua especificado; se o teste não acontecer até lá, a apresentação usa um celular do sistema já aprovado.
  - ~~Os dois planos em aberto até testar o outro sistema~~ — a integração pararia à espera de um celular (13/09).
  - ~~Veredito parcial vale como completo~~ — a R24 aceitou o parcial para o teste acontecer, não para dispensar o outro sistema (13/09).

### Rodada 11 — R33–R35 (13/09) — o banco na nuvem

Nasce da issue #21, o banco no Supabase. Escrever as regras de acesso mostrou que o contrato do §7 não se sustentava: o endereço de leitura deixou de funcionar com a R20, que tirou a turma de `alunos`, e regras de linha sobre tabelas abertas não entregam o que a issue #1 decidiu sobre a chave do aparelho. As três decisões são do Caio, fechadas na preparação da #21.

- **R33 O aparelho só entra no banco por duas funções.** A chave gravada no aparelho não tem privilégio em tabela nenhuma. Ela executa `roster(turma)`, que devolve matrícula, nome e UID dos alunos ativos de uma turma, e `enviar_relatorio(relatorio)`, que recebe o JSON do Compartilhar (§7) e grava sessão e eventos numa transação. As duas moram no próprio Postgres, e o `PostgREST` as expõe em `/rest/v1/rpc/`, então continua não havendo servidor para escrever (R21). O SQL fica versionado em `supabase/`, em arquivos numerados aplicados pelo editor de SQL do painel; arquivo aplicado não se edita, e mudança vira o arquivo seguinte. **Consequências:**
  - **O duplo Enviar (HU-39).** A sessão só chega ao banco no Enviar, já `finalizada`. Se o `sessaoId` já existe, a função responde `ja_enviado` e não grava nada. O aluno atrasado que encoste depois de um Enviar bem-sucedido fica fora de um segundo envio; é a lacuna do estado depois do Enviar (§8), decidida na integração.
  - **O `sessaoId` passa a ser `<turma>-<AAAAMMDD>-<HHMM>`**, com o relógio do Iniciar (§7). No formato do exemplo antigo, duas turmas no mesmo dia teriam o mesmo id, e o relatório da segunda seria recusado como já enviado.
  - **Enriquecimento na chegada (R3).** Evento com UID e sem matrícula recebe a matrícula do dono daquele UID. O campo `enriquecido` guarda o que o aparelho mandou, para o histórico mostrar que a aula foi offline; UID sem dono fica com a matrícula vazia.
  - **A matrícula digitada à mão não é conferida** contra o cadastro, porque um erro de digitação do professor não pode recusar o relatório inteiro. O formato do UID é conferido no cadastro de `alunos`, onde um erro impediria o crachá de ser reconhecido.
  - **Apagar e alterar dão erro de permissão.** Sem privilégio na tabela, o Postgres recusa antes de olhar as linhas; uma regra de linha sozinha responderia sucesso com zero linhas, e a recusa ficaria invisível.
  - **Custo assumido, a declarar no relatório técnico.** Quem extrair a chave lê nome, matrícula e UID de uma turma cujo código conheça, e o da turma real está neste repositório público. Consegue também enviar o relatório falso de uma aula nova. Não lê a lista de turmas nem o histórico, e não apaga nem altera nada, o que mantém o pior caso da issue #1: bagunçar uma aula.
  - **Dados de teste.** São inventados, salvo o nome e a matrícula dos cinco integrantes, e as aulas passadas têm id começando em `teste-`. Saem na integração, antes do primeiro relatório de aula real, junto com as sessões `verificacao-` que o `supabase/verificar.ps1` deixa a cada rodada.
  - ~~O aparelho lê e grava direto nas tabelas, filtrado por regras de linha (RLS)~~ — a chave leria a lista de todas as turmas, e apagar pareceria ter dado certo (13/09).
  - ~~Corpo do envio cru, em parâmetro sem nome~~ — funciona no `PostgREST` v14, mas é menos explícito e só se prova no Supabase de verdade (13/09).
- **R34 O histórico só se lê com login.** A página do Igor (R21) roda no navegador com a chave dentro, e quem tem o link tem a chave. A leitura passa a exigir e-mail e senha: com login, a chave vale como papel `authenticated`, que lê as seis tabelas e não grava nada. O cadastro público do Supabase fica desligado, e as contas, a do Igor e uma de demonstração, são criadas à mão pelo Caio. Com o cadastro ligado, qualquer um com a chave criaria uma conta e leria o histórico inteiro.
  - ~~Página aberta a quem tiver o link~~ — quem pega a chave, que vai dentro da página, leria nome e presença de todos os alunos, contra o ponto 4 da issue #1 (13/09).
- **R35 A chave do aparelho é a publicável.** O Supabase aposenta as chaves `anon` e `service_role` até o fim de 2026. A chave publicável (`sb_publishable_…`) viaja só no cabeçalho `apikey`, nunca em `Authorization: Bearer`, porque não é um token JWT, e no `secrets.h` se chama `SUPABASE_PUBLISHABLE_KEY`. Sem login ela vale como papel `anon`.
  - ~~Chave `anon` legada~~ — está sendo aposentada até o fim de 2026, e trocar de chave no meio da integração custaria mais do que começar com a nova (13/09).

### Rodada 12 — R36–R41 (15/09) — o que as entregas do time decidiram

Nasce da conferência das entregas de 14 e 15/09: o pull request das telas do professor (#29, Igor) e os quatro programas de bancada (#24 a #27, Cauã). Duas das seis decisões são propostas do Cauã, aceitas e renumeradas; três nascem da conferência; uma fecha o fluxo de trabalho que o pull request abriu. Todas são do Caio.

- **R36 No disco, cada evento é uma linha; o array do §7 se monta na saída.** O `eventos.json` é gravado por acréscimo, uma linha de JSON por evento, e nunca reescrito. O array que o §7 mostra é montado no momento de exibir, compartilhar ou enviar o relatório — o contrato de saída não muda um caractere. Proposta do Cauã na #27 em 15/09, com a varredura do passo 2 do `AGENTS.md` já feita. **Por quê:** a releitura fica barata, uma linha de cada vez em documento estático, que é a razão de existir da R29; e o custo de escrita e o desgaste da flash cresceriam com o tamanho da chamada a cada evento, porque reescrever o array significa reescrever tudo o que já foi registrado. **Consequências:**
  - Linha truncada por queda de energia é descartada na releitura, e o programa sinaliza que a descartou.
  - O evento vai para o arquivo **antes** de o UID entrar na memória de quem já está registrado (R30), para uma falha de gravação não deixar o aparelho achar que registrou alguém que o arquivo não conhece.
  - ~~O array reescrito a cada evento deixaria o arquivo impossível de abrir se a energia caísse no meio da reescrita~~ — razão oferecida na proposta e **corrigida na fonte**: o littlefs grava em cópia e volta ao último estado bom quando falta energia, então o arquivo voltaria ao conteúdo anterior, e não ficaria ilegível. A decisão continua de pé pelas outras duas razões (15/09).
  - A proposta chegou numerada como "R33", número que a rodada 11 tinha acabado de usar. O próprio Cauã avisou que a numeração era chute e mandou conferir no cabeçalho da spec, como o `AGENTS.md` pede.
- **R37 O campo de horário do arquivo é `ms_desde_boot`.** Nenhuma chave `timestamp` entra no `eventos.json` enquanto não houver relógio. O horário de verdade chega do navegador do professor no Iniciar (`clock.cpp`, §5), e gravar `timestamp` com o tempo desde o boot faria o arquivo prometer uma coisa e entregar outra. Proposta do Cauã na mesma passada. O `timestamp` do §7 continua sendo o que sai no relatório; a conversão é da saída, e está declarada como lacuna no §8.
- **R38 A medição dos 200 ms começa antes da conversa com o RC522, e o número é piso.** O programa imprime dois tempos: o da decisão, que mede da leitura do UID até o feedback começar, e o do toque, que começa antes de o firmware perguntar ao RC522 se há tag no campo. O critério do teste 1 usa o segundo. **Por quê:** entre a leitura do UID e o feedback há uma consulta a um vetor, e o primeiro número sai perto de zero em qualquer circunstância — o critério nunca dispararia, e o §5 e o §10 falam do encostar até o feedback, que é onde o tempo mora. **A objeção do Cauã na #26 estava certa, e o corpo da issue é que estava errado.** Nem o segundo número é o total: o tempo em que a tag espera no campo até o `loop()` buscá-la não é medível de dentro do aparelho, então ele é **piso**, e se declara como tal em toda evidência. **Gatilho de conferência:** piso acima de 100 ms manda medir o período do `loop()` antes de qualquer veredito, porque é ele que ocupa a distância até os 200 ms.
- **R39 O pull request vale quando o autor quer que uma decisão possa cair antes de entrar.** Commit direto na `main` continua sendo o padrão do §9. O pull request é a exceção, com uma razão só, e é a razão do Igor na #29: ele decidiu por conta própria pôr as três telas numa página, e quis que essa escolha pudesse ser derrubada **antes** de entrar na `main`, não depois. Revisão obrigatória **não** se adota: com uma placa só, o João proibido de editar arquivo versionado (R28) e nove execuções físicas em onze dias, o processo só somaria espera.
  - ~~Todo trabalho passa a entrar por pull request~~ — o número real de mãos no código é uma ou duas, e a modularização já faz o trabalho que o processo faria (R9, R28); a espera custaria mais do que o conflito que ela evita (15/09).
- **R40 Nenhuma tela do portal avança sem o aparelho confirmar.** Todo pedido ao aparelho tem tempo-limite. Estourado o limite, a falha **aparece na tela**, o botão volta a aceitar toque e nenhuma tela troca. A mensagem diz que o aparelho se recupera sozinho (R15), para o professor esperar e tentar de novo em vez de desistir. Nasce de um defeito do protótipo: o botão *Seguir sem a lista*, que existe justamente para o caso de o aparelho não responder, pedia `/api/seguir-offline` e só trocava de tela dentro do `.then()` — com o aparelho mudo, ele não fazia nada e não avisava nada. O mesmo desenho estava no *Voltar ao login*.
  - ~~Passado o tempo-limite, a tela segue em frente sem a confirmação~~ — o painel também vive de sondagem, e o professor cairia numa tela congelada que diz que a chamada está aberta quando o aparelho não confirmou nada: trocaria "não acontece nada" por "acontece a coisa errada" (15/09).
- **R41 A partição é uma só, e o firmware integrado não a formata.** O portal (`data/`, R27) e o `eventos.json` moram na **mesma** partição de LittleFS. Duas consequências, que são o mesmo problema visto de dois lados:
  - O `platformio.ini` declara **`board_build.filesystem = littlefs`**. Sem essa linha o padrão do espressif32 é SPIFFS: o `pio run -t uploadfs` grava uma imagem SPIFFS, o firmware tenta montar LittleFS, a montagem falha — e formata.
  - `LittleFS.begin(true)` **fica** nos programas de bancada, justificado no código: lá a partição só tem o `eventos.json`, e formatar é o que destrava a placa. No firmware integrado o `true` sai, porque formatar apagaria o portal recém-gravado. Montagem que falhar no aparelho de verdade é erro a mostrar, não partição a apagar.

### Rodada 13 — R42–R43 (16/09) — o que o aparelho escreve no banco e o que fica na saída

Nasce do pente-fino do repositório em 16/09, uma leitura das doze rodadas contra o código, os tickets e as issues. As duas divergências abaixo não são resíduo de redação: são consequências da R33 que ninguém declarou, e por isso o §7 seguia prometendo o desenho anterior. As duas decisões são do Caio.

- **R42 O aparelho só escreve `finalizada`; `aberta` e `cancelada` ficam para uso administrativo.** Dos três valores de `sessoes.estado` criados na R20, a R33 deixou um só alcançável pelo aparelho: a sessão não existe no banco enquanto a aula corre, chega no Enviar e o `enviar_relatorio` grava `'finalizada'` literal. A idempotência do segundo Enviar, que a R20 atribuía ao estado, quem faz hoje é a chave primária `sessaoId`, com `on conflict do nothing` (R33) — no caminho do aparelho o estado não decide nada, e o `default 'aberta'` da coluna nunca chega a valer, porque a função nomeia o estado no `insert`. **A coluna continua com os três valores**, porque os outros dois têm dono: `aberta` e `cancelada` são escrita administrativa, de quem corrige uma aula à mão pelo painel do Supabase — nem a chave do aparelho nem a conta de login (R34) gravam em tabela, e o V7 do `verificar.ps1` prova a recusa; e o `aberta` pode voltar a ser escrito pelo aparelho se a lacuna do §8 "estado depois do Enviar" for decidida, na integração, pela sessão que continua aberta depois de um Enviar bem-sucedido. **Consequências:**
  - **Os dados de teste ficam como estão.** A aula `cancelada` de 01/09 do `002-dados-de-teste.sql` é semeada à mão e nenhum aparelho a produziria — ela existe para a página do histórico (R21) ter os três estados para mostrar, e é por isso que não recebe evento nenhum. O `002` ganha a linha que diz isso, e o `001` a ganha ao lado da `check`, porque quem lê só o esquema entende que o aparelho escreve os três.
  - **A página do histórico continua renderizando os três estados**, e não precisa tratar `aberta` e `cancelada` como impossíveis. O que ela pode assumir é o contrário: estado diferente de `finalizada` não veio do aparelho.
  - **Nenhum DDL muda.** A `check` e o `default` ficam como estão: arquivo aplicado não se edita (R33), e aqui não há o que corrigir, só o que declarar — o que entra nos dois arquivos é comentário.
  - ~~Podar a coluna para o único valor que o aparelho escreve~~ — tiraria do banco a correção administrativa e fecharia antes da hora a lacuna do §8, que ainda pode reabrir o `aberta` (16/09).
- **R43 `tempos` e `roster_carregado` valem para o professor e para o Compartilhar, e não viram coluna.** A exportação do §7 é contrato com o banco **e** com o professor, e os dois públicos não levam as mesmas chaves: o `enviar_relatorio` grava sessão e eventos e descarta `tempos`, `roster_carregado`, `presentes` e `faltantes`. É desenho, não esquecimento, e passa a estar declarado. `tempos` é `["t1", "t2"]` constante pela Q8 — uma chamada cobre os dois tempos da aula, e a coluna repetiria o mesmo par em toda linha. `roster_carregado` se lê de `sessoes.modo`, porque o relatório sai sem as listas exatamente quando o login foi offline (R14). `presentes` e `faltantes` o banco recalcula de `eventos` cruzado com `alunos_turmas`, como o §7 já dizia. As quatro continuam na exportação porque a tela do Compartilhar e o arquivo salvo no celular do professor precisam se explicar sozinhos, sem banco do lado. **Nada muda no SQL nem no JSON** — o §7 ganha a frase que separa o que atravessa do que para na saída.
  - ~~Acrescentar `tempos` e `roster_carregado` como colunas de `sessoes`~~ — guardaria um par constante repetido em toda linha e um valor que já se lê do `modo`, e ainda custaria um `003-`, porque arquivo aplicado não se edita (16/09).

### Rodada 14 — R44 (21/09) — a checagem do buzzer termina no transistor

Nasce da prova de vida do LED e do buzzer (#24), a primeira vez que as peças obedeceram ao firmware. A checagem do buzzer (#23) não teve comentário próprio: o resultado veio no relatório da #24 e pelo WhatsApp do João, repassado pelo Caio.

- **R44 O buzzer entra por transistor, e a peça atual precisa ser trocada.** Fecha a checagem da R22. O João ligou o buzzer por um transistor NPN Darlington **TIP122**, com a peça alimentada pelos 5 V do pino VIN e o sinal saindo do GPIO 33. Com o programa da prova de vida, as três cores do LED bateram com o monitor serial, e o bip saiu muito baixo, com um som estranho, audível só de perto. Para o João, a peça está ruim e precisa ser comprada de novo; para a demonstração de 22/09, ela serve de perto. **Consequências:**
  - O transistor fica, e o §4 e o §4.1 passam a descrever a ligação montada. O TIP122 é NPN, como a R22 pedia, embora não seja um dos três que ela citava. O valor do resistor da base não foi informado.
  - **Antes de comprar, conferir se a peça é ativa.** O firmware liga o buzzer com nível alto constante (`lib/feedback/`), e isso só produz som em buzzer **ativo**, o que tem oscilador interno. Um buzzer **passivo** sob nível constante dá um estalo baixo ao ligar e ao desligar, que combina com o que o João descreveu. A conferência é ligar a peça direto entre o VIN (5 V) e o GND: o ativo apita alto e contínuo, e o passivo só estala. Se for passivo, a compra é de um buzzer ativo de 5 V, e o firmware não muda. É hipótese levantada na conferência de 21/09, ainda não verificada na peça.
  - **O LED foi conferido na mesma rodada.** A peça é RGB de catodo comum, porque as três cores acenderam com a polaridade que o programa já tinha, sem inverter a constante. A montagem usa resistores de 300 Ω, e não os 220 Ω do §4.1; com eles as três cores acenderam, e a tabela passa a registrar o valor montado.

### Decisões operacionais

- **Bluetooth não será usado** — substituído pelo Wi-Fi nativo do ESP32 (o enunciado da Entrega 02 pedia Bluetooth por boilerplate de Arduino).
- **Aparelho fixo na sala** (decisão do professor + grupo) → alimentação por fonte 5V, caixa de parede, contas de vários professores no mesmo aparelho.
- **O prazo de cada entrega é o do enunciado do professor.** Datas sem enunciado publicado vêm do cronograma passado ao grupo em 05/09 (22/09, 29/09, 05/10, 20/10, 27/10, 30/11) e se conferem quando o enunciado sair. Até 13/09 só as Entregas 03 e 04 tinham enunciado, e as duas citam **21/09** como a data da preparação. O da Entrega 05 saiu em 20/09 e fixou **22/09, 23:59**, com demonstração presencial. ~~Cronograma válido = o revisado (datas de terça: 07/09, 14/09, 22/09, 29/09, 05/10, 20/10, 27/10, 30/11); as datas de segunda são da outra turma~~ — informação errada: 07/09, 14/09, 05/10 e 30/11 caem numa segunda (13/09).

## 3. Arquitetura em blocos

Fonte editável do diagrama: `docs/diagrama-blocos.drawio` (abre no draw.io), com a imagem exportada, usada nos documentos de entrega, em `docs/diagrama-blocos.png`. O ASCII abaixo é a referência rápida e deve ser mantido em sincronia com ele. O **fluxograma da lógica** do §5 é arquivo separado — `docs/fluxograma-logica.drawio` (R27) — e nasce na Entrega 04.

```
[Tag NFC 13,56 MHz] --RF--> [RC522] --SPI VSPI--> [ESP32 DevKit V1]
                                                        |
                                                        |--GPIO 25/26/27--> [LED RGB]
                                                        |--GPIO 33-------->  [Buzzer ativo]
                                                        |--LittleFS------->  professores.json / sessao_atual.json / eventos.json
                                                        |
                                            +-----------+-----------+
                                     modo AP (aula)          modo STA (2 momentos)
                              ao mesmo tempo (Plano A) ou alternando (Plano B) — R19
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
| ESP32 DevKit V1 / 32D | 1 | Controlador principal, Wi-Fi (AP+STA ou alternado — R19), WebServer, LittleFS | ✅ adquirido |
| Leitor RFID/NFC RC522 13,56 MHz | 1 | **Sensor** — leitura do UID (SPI VSPI: SCK 18, MISO 19, MOSI 23, SS 5, RST 22) | ✅ adquirido |
| Tags NFC 13,56 MHz (NTAG215 + cartões brancos do kit) | 3–5 | Crachás de teste — firmware só lê o UID | ✅ adquirido |
| LED RGB catodo comum + 3× resistor (300 Ω na montagem; a lista previa 220 Ω) | 1 | **Atuador 1** — verde/vermelho/azul (GPIO 25/26/27) | ✅ montado e conferido em 21/09 (#24) |
| Buzzer ativo 5V | 1 | **Atuador 2** — bip 100 ms na confirmação (GPIO 33) | ⚠️ montado; som fraco, a peça será trocada (R44) |
| Transistor NPN TIP122 | 1 | Chaveia o buzzer de 5 V a partir do GPIO 33 (R44) | ✅ montado em 21/09 |
| Protoboard 830 pontos | 1 | Montagem | ✅ adquirido |
| Jumpers M-M e M-F | 1 kit | Conexões | ✅ adquirido |
| Fonte 5V 2A / cabo micro-USB | 1 | Alimentação (aparelho fixo, sempre na tomada) | ✅ adquirido |

✅ **Lista fechada em 10/09/2026 (R22):** o João comprou tudo e as peças estão com ele. ~~Não há compra pendente.~~ ⚠️ **Uma compra reabriu em 21/09 (R44):** um buzzer novo, depois de conferir se a peça atual é passiva.

⚠️ **Atenção elétrica:** RC522 é estritamente **3,3V** — 5V queima o módulo. Buzzer ativo 5V: a ligação ao GPIO depende da checagem abaixo, e o transistor só entra no fim dela. **Ordem de checagem na bancada, antes de comprar transistor (R22):** (1) se o buzzer for módulo de 3 pinos, o driver já está embutido e não falta nada; (2) se for peça solta de 2 pinos, alimentar em 3,3 V e ouvir — o bip precisa ser audível a um passo do aparelho, não alto; (3) só então o NPN, e serve qualquer um (2N2222, BC337, S8050) de loja física. ⚠️ **Fechado em 21/09 (R44):** a checagem terminou no passo 3, com um TIP122.

Sem o RTC, o barramento I2C some e o conflito de pino SDA 21 / SCL 22 vs RST 22 que existia na versão anterior **deixa de existir**.

### 4.1 Ligações do ESP32

| Componente | Pino do módulo | GPIO | Direção | Observação |
|---|---|---|---|---|
| RC522 (sensor) | SDA / SS | 5 | Saída (SPI CS) | GPIO 5 é pino de *strapping*: precisa estar em nível alto no boot. É o repouso natural do CS, mas se a placa não subir, conferir esta linha antes de qualquer outra |
| RC522 | SCK | 18 | Saída | VSPI por hardware |
| RC522 | MOSI | 23 | Saída | |
| RC522 | MISO | 19 | Entrada | |
| RC522 | RST | 22 | Saída | |
| RC522 | 3,3V / GND | — | Alimentação | ⚠️ **estritamente 3,3 V** — 5 V queima o módulo |
| LED RGB (atuador 1) | R | 25 | Saída | resistor em série: 300 Ω na montagem de 21/09, onde a spec previa 220 Ω (R44) |
| LED RGB | G | 26 | Saída | resistor em série, idem |
| LED RGB | B | 27 | Saída | resistor em série, idem; catodo comum ao GND, conferido em 21/09 (#24) |
| Buzzer ativo 5V (atuador 2) | sinal | 33 | Saída | **por transistor NPN TIP122 (R44):** GPIO 33 → resistor → base, emissor no GND, coletor no negativo do buzzer, positivo do buzzer em 5 V (VIN). O resistor da base não foi informado; o valor de referência é 1 kΩ. ~~A ligação sai da checagem de bancada (R22)~~: a checagem terminou no transistor (21/09) |
| Alimentação | — | — | — | fonte 5 V 2 A na tomada (aparelho fixo); 5 V para o buzzer, 3,3 V do regulador da placa para o RC522 |

Nenhum GPIO acumula duas funções, e nada disputa os pinos de boot além do CS acima.

## 5. Firmware (C++, Arduino Core sobre PlatformIO)

Build: **PlatformIO** no VS Code (`platformio.ini` fixa as versões de biblioteca para os 5; quais e por quê: R29). Libs: `MFRC522` · `DNSServer` · `WebServer` · `LittleFS` · `ArduinoJson` · `HTTPClient` + `WiFiClientSecure` · `mbedtls` (SHA-256). Credenciais em `secrets.h`, fora do git.

Até 22/09, os programas de bancada moram cada um na sua pasta de `src/`, com o seu ambiente (R28). A tabela abaixo descreve o firmware de verdade.

Módulos em `src/`:

| Arquivo | Responsabilidade |
|---|---|
| `config.h` | Pinos e tempos. SSID e senha do AP, credenciais do hotspot e URL e chave da API vêm do `secrets.h` (R11, R17) |
| `storage.cpp` | LittleFS: professores, sessão atual, append de eventos, quem já está registrado na sessão (R30). No disco cada evento é **uma linha**, com o horário em `ms_desde_boot`; o array do §7 se monta na saída (R36, R37) |
| `rfid.cpp` | Leitura de UID 4/7 bytes, janela de silêncio de 5 s (R30) |
| `clock.cpp` | Hora recebida do navegador no Iniciar (ou da API), offset sobre `millis()` |
| `portal.cpp` | Login, cookie de sessão, rotas HTTP |
| `net.cpp` | Rádio: `WIFI_AP_STA` no Plano A; no Plano B, alternância AP↔STA (sequência fixa: parar server+DNS → `softAPdisconnect(true)` → `WIFI_OFF` → `delay(500)` → `WIFI_STA`; e o inverso na volta). Roster, upload, retry **com limite** |
| `feedback.cpp` | LED + buzzer por máquina de estados com `millis()` — **sem `delay()`** |

**Enquanto o `config.h` não existir, a pinagem do §4.1 mora em dois lugares:** `lib/feedback/feedback.cpp` (LED 25/26/27 e buzzer 33) e `lib/rfid/rfid.cpp` (SS 5 e RST 22). Mudança de pino toca os dois, e conferir só um deixa a placa meio ligada pelo mapa novo e meio pelo antigo. O `config.h` nasce com o firmware integrado; programa de bancada nenhum precisa dele.

Regras de performance: loop sem `delay()`; SPI em VSPI por hardware; `ArduinoJson` com documento estático (sem concatenar `String`); log serial com níveis.

### Máquina de estados

| Estado | LED | Som | Gatilho |
|---|---|---|---|
| `AGUARDANDO_LOGIN` | azul fixo | — | boot / sessão encerrada |
| `SINCRONIZANDO` | azul lento | — | rádio em STA (após o login, ou no Enviar) |
| `FALHA_DE_RADIO` | azul rápido | — | AP não voltou em 2 tentativas → `ESP.restart()` (só no Plano B) |
| `SESSAO_ABERTA` | verde fixo | — | professor clicou Iniciar |
| `REGISTRADO` | verde 2 piscadas | bip 100 ms | UID lido e gravado |
| `JA_REGISTRADO` | verde 1 piscada | — | UID já registrado nesta sessão (R30) — nada é gravado |
| `NAO_RECONHECIDO` | vermelho 1 s | buzz 400 ms | **só em modo online**: UID fora da lista da turma — nada é gravado |

Em **modo offline** o estado `NAO_RECONHECIDO` não existe: sem lista da turma, todo UID lido é aceito e vira `REGISTRADO`. O LED verde passa a significar *"li a tag"*, não *"você está nesta turma"*.

O crachá parado no leitor não repete sinal: leituras do mesmo UID até 5 s depois da anterior são ignoradas sem feedback nenhum, e só depois disso um novo toque de quem já está registrado dá `JA_REGISTRADO` (R30).

### Pseudocódigo

Renderização fiel da máquina de estados acima e do fluxo do §6: o que o firmware faz, não como cada módulo faz. O `loop()` nunca bloqueia — sem `delay()`, tudo temporizado por `millis()`. Os passos marcados `só no Plano B` são a alternância de rádio e deixam de existir se a bancada aprovar o Plano A (R19); o fluxograma marca os mesmos passos com `*`.

```
INÍCIO (boot)
  inicializar LittleFS, SPI, RC522, LED e buzzer
  SE existe sessao_atual.json E a janela ainda está aberta:
      retomar a sessão (mesmo professorId, mesmo roster)        // R16
      reler eventos.json: quem já está registrado               // R30
      estado <- SESSAO_ABERTA                                   // R16: a janela ainda está aberta
  SENÃO:
      estado <- AGUARDANDO_LOGIN                                // LED azul fixo
  subir SoftAP (WPA2) + DNS do portal cativo + WebServer

ENQUANTO ligado:                                                // loop(), sem delay()
  atender as requisições do portal
  atualizar LED e buzzer pelo tempo decorrido (millis)

  SE o professor autenticou:
      responder "carregando turma..."
      estado <- SINCRONIZANDO                                   // R14: sincroniza no login
      fechar o AP e trocar o rádio para STA                     // R13a: heap do TLS · só no Plano B
      baixar o roster da turma
      SE conseguiu:  gravar o roster em cache;  modo <- online
      SENÃO:                                     modo <- offline // R3
      voltar o rádio para AP                                    // só no Plano B
      SE o AP não voltou em 2 tentativas:                       // só no Plano B
          estado <- FALHA_DE_RADIO; reiniciar                    // R15 · só no Plano B
      abrir o painel com a lista já em cache                    // estado seguinte: lacuna do §8

  SE o professor clicou Iniciar:
      abrir a sessão e gravar sessao_atual.json
      estado <- SESSAO_ABERTA                                   // LED verde fixo

  SE o RC522 leu uma tag nova E a sessão está aberta:  // nova: fora da janela de 5 s do mesmo UID (R30)
      uid <- UID lido (4 ou 7 bytes, somente leitura)
      SE uid já está registrado nesta sessão:
          estado <- JA_REGISTRADO                // verde 1 piscada, nada gravado · R30
      SENÃO SE modo = online E uid não está no roster:
          estado <- NAO_RECONHECIDO              // vermelho 1 s + buzz, nada gravado
      SENÃO:
          gravar evento (uid, matrícula e nome se houver, horário, origem "nfc")
          estado <- REGISTRADO                   // verde 2 piscadas + bip 100 ms
      // do encostar da tag até o feedback: alvo < 200 ms

  SE o professor lançou presença manual:
      SE modo = online E a matrícula já está presente:
          recusar no portal                                      // R30
      SENÃO:
          gravar evento com origem "manual", lancado_por e motivo  // Q6

  SE o professor clicou Enviar:
      estado <- SINCRONIZANDO
      fechar o AP antes de abrir o TLS                           // R13a · só no Plano B
      enviar sessão + eventos para a API, com tentativas limitadas  // R13b
      SE falhou: manter eventos.json e deixar o Compartilhar disponível
      voltar o rádio para AP                                    // só no Plano B
      SE o AP não voltou em 2 tentativas:                       // só no Plano B
          estado <- FALHA_DE_RADIO; reiniciar                    // R15 · só no Plano B
      // estado seguinte ao Enviar: lacuna do §8

  SE o professor clicou Encerrar:
      fechar a sessão e apagar o roster em cache                 // LGPD por desenho
      estado <- AGUARDANDO_LOGIN
FIM
```

## 6. Portal do professor — fluxo canônico (16 passos)

HTML/CSS/JS vivem como arquivos no LittleFS (imagem de filesystem separada do firmware), servidos pelo `WebServer`. A pasta versionada é **`data/`** na raiz do repo — é dela que o PlatformIO gera a imagem de filesystem que sobe para a placa (R27). Ajuste de layout não exige recompilar o firmware.

1. Professor liga o aparelho
2. Aparelho sobe; professor procura as redes Wi-Fi
3. Professor conecta na rede do aparelho (senha WPA2)
4. Portal cativo abre a página de login automaticamente
5. Professor entra com login e senha. Validada a senha, o aparelho responde *"carregando turma…"* e **aproveita esse instante para sincronizar** (R14): no Plano A, o STA busca o roster com o AP no ar, e o celular não perde a conexão; no Plano B, o rádio vai a STA, busca o roster e volta a AP em 5–15 s, o celular reconecta sozinho e o LED pisca azul lento (R19)
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

**As rotas entre o portal e o aparelho ainda não são contrato.** O protótipo da #20 fala com um aparelho de mentira (`data/mentira.js`), que responde oito rotas `/api/*` e um punhado de códigos de erro. São **provisórias**: foram desenhadas contra um arquivo de JavaScript, e não contra o `WebServer`, a memória e o portal cativo do aparelho. O contrato de verdade fecha no `portal.cpp`, e está declarado como lacuna no §8. Enquanto isso, `data/mentira.js` é onde as rotas do protótipo se leem.

**Nenhuma tela avança sem o aparelho confirmar** (R40): pedido que estoura o tempo-limite mostra a falha, libera o botão e não troca de tela.

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
```

Exportação:

```json
{
  "sessao": { "sessaoId": "4172CMPN6A_P1-20260907-1900", "professorId": "prof02",
              "disciplina": "Sistemas Embarcados", "turma": "4172CMPN6A_P1",
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
  "faltantes": ["1240108001"]
}
```

**O arquivo no disco não é este array.** O `eventos.json` é gravado por acréscimo, **uma linha de JSON por evento**, e o campo de horário nele se chama `ms_desde_boot` — não `timestamp`, que só existe depois que o relógio do Iniciar chega (R36, R37). A exportação acima é montada na hora de exibir, compartilhar ou enviar, e é o que vale como contrato com o banco e com o professor.

`presentes` e `faltantes` só existem quando `roster_carregado: true`. Em modo offline o relatório sai só com `eventos`, e as listas são calculadas do lado da API no momento do envio.

**`tempos` e `roster_carregado` não viram coluna no banco** (R43). A exportação é contrato com o banco **e** com o professor, e o `enviar_relatorio` guarda só o que não sabe recalcular: `tempos` é constante pela Q8, `roster_carregado` se lê de `sessoes.modo`, e `presentes` e `faltantes` saem de `eventos` cruzado com `alunos_turmas`. As quatro chaves continuam na exportação porque a tela do Compartilhar e o arquivo salvo no celular do professor precisam se explicar sem banco do lado.

**`sessaoId` = `<turma>-<AAAAMMDD>-<HHMM>`**, com o relógio do Iniciar. É por ele que o banco reconhece o segundo Enviar, então precisa ser único entre turmas (R33). ~~`20260907-T1`~~ — duas turmas no mesmo dia teriam o mesmo id (13/09).

**Supabase:** tabelas `alunos`, `professores`, `turmas`, `alunos_turmas`, `sessoes` (com `estado`: `aberta` / `finalizada` / `cancelada`) e `eventos` — desenho fechado em R20, na issue #1. **O aparelho só escreve `finalizada`** (R42): a sessão chega ao banco no Enviar, já encerrada. `aberta` e `cancelada` são escrita administrativa, e o `aberta` pode voltar a ser escrito pelo aparelho quando a lacuna do §8 "estado depois do Enviar" for decidida. ⚠️ **A issue chamou `sessoes` de `aulas`: é a mesma entidade.** O nome no código e no banco continua `sessoes`, para não quebrar o contrato de dados acima, o `sessao_atual.json` (R16) nem o glossário. ~~Chave anon + RLS. O ESP32 faz `GET /alunos?turma=eq.<turma>` no login (R14) e `POST /sessoes` + `POST /eventos` no Enviar.~~ ⚠️ **Substituído em R33–R35 (13/09):** o endereço de leitura não funcionava desde a R20, que tirou a turma de `alunos`. O ESP32 chama duas funções, com a chave publicável só no cabeçalho `apikey` (R35): no login (R14), `POST /rest/v1/rpc/roster` com `{"turma": "<turma>"}`; no Enviar, `POST /rest/v1/rpc/enviar_relatorio` com `{"relatorio": <a exportação acima>}`, que responde `gravado` ou, se a sessão já tiver chegado, `ja_enviado` (R33). O esquema completo está em `supabase/001-esquema.sql`. Falha de rede → o relatório fica em `eventos.json` e o botão Compartilhar continua funcionando.

## 8. Plano de testes

**Como testamos.** As decisões de teste, antes da tabela:

- **O ambiente de teste é a bancada, com o aparelho real.** Não há teste automatizado, nem em host (`pio test` com ambiente `native`). É escolha, não omissão: o que pode dar errado aqui é físico e de rádio — a leitura SPI do RC522, a alternância AP↔STA que não volta, o comportamento de cada celular no pulo de canal do `WIFI_AP_STA` (R19), os 40–50 KB de heap do handshake TLS, a integridade do LittleFS numa queda de energia. Nada disso aparece contra um mock; o teste de host testaria o mock, e a camada de abstração necessária para escrevê-lo custaria mais do que a bancada custa.
- **Só comportamento externo é testado**, nunca função interna: o que o LED e o buzzer fazem, o que o portal mostra, o que sai no JSON exportado, o que o log serial registra. Teste que precisa abrir o código para saber se passou não é teste deste projeto.
- **Todo teste entrega evidência gravada** — foto ou vídeo curto do LED e do buzzer, o JSON exportado, ou o trecho do log serial com horário. É o que sustenta o entregável de 22/09 ("testes documentados") e o relatório de 30/11. A evidência **mora no repositório**, em `docs/assets/testes/`, consolidada em `docs/testes-22-09.md` (R27): o que fica só em comentário de issue some da vista do professor.
- **O log serial com níveis (§5) é instrumento de medição**, não resto de depuração: sem ele, os <200 ms e o diagnóstico da troca de rádio viram opinião.
- **Tempo medido de dentro do aparelho é piso, e se declara como tal.** Os 200 ms do teste 1 se contam do início da conversa com o RC522, e não da leitura do UID já pronto — entre o UID e o feedback só há uma consulta a um vetor, e o número sairia perto de zero sem provar nada (R38). Nem esse começo é o zero verdadeiro: o tempo em que a tag espera no campo até o `loop()` ir buscá-la não é visível daqui. Piso acima de 100 ms manda medir o período do `loop()` antes do veredito, porque é ele que ocupa a distância até os 200 ms.
- **Quem fecha uma frente roda os testes que a tocam**; a rodada completa acontece na integração, antes de 22/09, e se repete antes de cada marco com demonstração ao vivo (20/10 e 27/10).
- **Calibração faz parte do teste, não do conserto.** Os tempos (janela de silêncio de 5 s, bip de 100 ms, piscada do `JA_REGISTRADO`, vermelho de 1 s) e a janela de 5–15 s da troca de rádio são valores de partida. Espera-se ajustá-los com o aparelho montado na caixa e alimentado pela fonte definitiva — o teste é o que diz para onde ajustar.

| # | Teste | Critério |
|---|---|---|
| 1 | UID 4 bytes (Mifare), 3 aproximações seguidas | 1 evento só (R30); **<200 ms contados do início da conversa com o RC522**, declarados como piso (R38) |
| 2 | UID 7 bytes (NTAG215) | registra pelo UID, sem escrita na tag |
| 3 | Modo online, tag fora da turma | vermelho longo, nada gravado |
| 4 | Modo offline (login sem internet, R14) | `modo: "offline"`, aceita todo UID, relatório íntegro |
| 5 | Enviar sem internet → depois com internet | Compartilhar funciona; retry sobe o mesmo relatório sem duplicar |
| 6 | 2 professores em sequência | 2 sessões, `professorId` distintos, sem mistura |
| 7 | **Plano B** — alternância AP↔STA, 10 ciclos seguidos | AP volta nas 10 vezes; celular reconecta; transição dentro de 5–15 s |
| 7b | **Plano B** — falha forçada na volta ao AP (hotspot desligado no meio) | aparelho se recupera sozinho e **não perde** as presenças já registradas |
| 7c | **Plano A** — `WIFI_AP_STA`: professor logado no portal, STA conecta no hotspot em outro canal | painel continua aberto **sem novo login**, em iPhone **e** Android; se cair, reconecta sozinho em <5 s. Com dois celulares do mesmo sistema, roda assim mesmo: o veredito sai **parcial** e o outro sistema fecha depois (R24); parcial e aprovado, adota o Plano A provisoriamente (R32) |
| 7d | **Plano A** — heap livre no instante do handshake TLS, com AP + DNS + WebServer + roster de pé | ≥40 KB livres (`ESP.getFreeHeap()` logado antes do `connect()`); abaixo disso, reprova e vale o Plano B. Medido com um celular só, sem cliente no AP, o número sai otimista — **40 a 45 KB é suspeito, não aprovado** (R26), e a memória que decide passa a ser a medida no 7c, com cliente conectado (R31) |
| 8 | Stress: 30 toques + reboot no meio | zero duplicata — um registro por crachá, e quem já estava registrado continua recusado depois do reboot (R30) —, zero perda (LittleFS persiste) |

**Lacunas conhecidas (11/09, ampliadas em 13/09 e em 15/09).** Dez itens **não têm task** e não vão ter antes da Entrega 05 — nenhum cabe na janela. Ficam listados para que a ausência seja escolha declarada, e não descuido:

| Item sem dono | Por que não virou task | Quando vira |
|---|---|---|
| Testes **3, 4, 5 e 6** | Todos dependem do roster **dentro** do aparelho, que só existe quando o ESP32 falar com a nuvem. Vivem no epic #9, ainda não fragmentado. Declarado na #28, mas nenhum ticket os possui | 29/09–05/10, na quebra do epic #9 |
| Testes **7 e 7b** (Plano B) | Só se tornam obrigatórios se a #19 der veredito **Plano B**; hoje a #19 emite o veredito e para aí | No dia do veredito, se der Plano B |
| **`sessao_atual.json`** (R16) | Requisito explícito da spec sem ticket — a #27 cobre só o `eventos.json` | Com a sessão de verdade, depois do portal (após 05/10) |
| **Máquina de estados completa** do §5 (`AGUARDANDO_LOGIN`, `SINCRONIZANDO`, `FALHA_DE_RADIO`, `SESSAO_ABERTA`) | As tasks de 22/09 cobrem só `REGISTRADO`, `JA_REGISTRADO` e o feedback; os estados de rede e de sessão dependem do portal (#20) e do veredito de rádio (#19) | Na integração, depois da Entrega 05 |
| **Estado depois do login e depois do Enviar** (§5) | O pseudocódigo põe o aparelho em `SINCRONIZANDO` e não diz para qual estado ele volta: com o painel aberto, o LED fica em azul lento até o Iniciar. Caso que a decisão tem de responder: o Enviar dá certo às 20h50 e um aluno atrasado encosta às 20h51 — a sessão continua aberta? O toque grava? Um segundo Enviar seria recusado pela sessão `finalizada` (R20)? Achado em 13/09 | Na integração, depois da Entrega 05, junto da máquina de estados completa |
| **Perguntas 2 e 3 do portal cativo** (ticket `.scratch/presente/issues/09`): o login fica na janelinha que o celular abre sozinho ou segue no navegador; o celular que vê "rede sem internet" e troca sozinho para o 4G | A R25 diz que a escolha se mede na bancada, mas nenhuma task a testa. A #18 só **observa** e anota o que aconteceu, sem decidir. Achado em 13/09 | Quando o portal (#20) rodar dentro do aparelho, depois da Entrega 05 |
| **De onde vem a turma no login** | O §6, o pseudocódigo e a R14 mandam baixar "a lista da turma", e a tela de login da #20 só pede usuário e senha. A função `roster` recebe o código da turma (R33), mas nada diz como o aparelho o obtém: escolhido pelo professor, deduzido do professor e do horário, ou gravado no aparelho. O protótipo da #20 resolveu fixando a turma no `data/mentira.js`, que é resposta de protótipo, não decisão. Achado em 13/09, na #21 | Junto do portal (#20), na integração depois da Entrega 05 |
| **Contrato portal↔firmware** | As oito rotas `/api/*` e os códigos de erro do protótipo (#20) nasceram contra um arquivo de JavaScript, e subir isso a contrato congelaria um desenho que nunca foi feito contra o `WebServer`, a memória e o portal cativo. Achado em 15/09, na conferência do PR #29 | No `portal.cpp`, na integração depois da Entrega 05 |
| **Converter `ms_desde_boot` em `timestamp` na saída** | A R37 tirou o `timestamp` do arquivo, e o §7 continua exigindo ISO-8601 na exportação. A conta é do `clock.cpp` (§5), que ainda não existe: a hora do Iniciar menos o tempo decorrido desde o boot. Achado em 15/09, na rodada 12 | Com o portal servindo o Iniciar de verdade, depois da Entrega 05 |
| **O firmware integrado não pode formatar a partição** | A R41 manda tirar o `true` do `LittleFS.begin()` quando o portal e o `eventos.json` dividirem a partição, e nenhuma task de hoje toca o firmware integrado. Achado em 15/09, na conferência da #27 | Na integração, junto da montagem dos módulos do §5 |


## 9. Divisão da equipe

| Papel | Responsável | Entregável até 21–22/09 |
|---|---|---|
| Hardware | Gabriel Santarello | Protoboard montada, pinagem conferida, alimentação estável |
| Firmware núcleo | Cauã Andrade | `rfid.cpp` + `storage.cpp` + `feedback.cpp` + máquina de estados |
| Portal web | Igor Lobato | SPA do professor: login, painel de 6 botões, polling do relatório |
| Integração e documentação | Caio Planinschek | Rede (AP↔STA), Supabase, roster + upload; consolidação dos documentos de entrega |
| Estrutura física e diário de bordo | João Victor Cabral | Caixa de parede, diário de bordo semanal, fotos e vídeo |

> [!WARNING] Divergência declarada em 11/09/2026 — a tabela acima é registro, não contrato
> A tabela foi entregue ao professor na Entrega 03 e por isso **fica como está**. O que ela descreve deixou de valer em 11/09, por **R23**: há um microcontrolador só, e a placa não circula — ela fica com o João, e o trabalho chega até ela mastigado. Duas linhas mudam de sentido:
>
> - **Hardware / Gabriel** — a protoboard montada passou a ser a issue **#22, do João**, porque as peças estão com ele. O Gabriel segue com o fluxograma da lógica (#10) e o dossiê de evidências (#28), escolhidos por não precisarem de placa nem de código e por não travarem terceiro.
> - **Estrutura física e diário / João** — deixou de ser "caixa de parede, diário, fotos e vídeo" e passou a ser **quem executa a bancada inteira**: toda task física nasce partida em *preparar* (qualquer um) e *rodar* (ele).
>
> A divisão que vale hoje são as **issues de atribuição do GitHub**, da #5 em diante, com dono por task, e o grafo de dependência em `.scratch/prototipo/graph.md`. Mesmo padrão do §1 (saída do controle de saída) e do §4 (dispensa do segundo sensor): a divergência se declara, não se apaga.

Cada papel tem 1 dono, mas as fronteiras são permeáveis: quem terminar a sua frente ajuda a próxima. O papel de **Integração** existe justamente para circular entre as camadas e fechar o que ficar entre duas cadeiras.

**Fluxo de trabalho no repo:** commits direto na `main`, sem branch nem pull request. A modularização do firmware (§5) é o que evita conflito — cada frente no seu arquivo. Até 22/09, entre programas de bancada, quem evita conflito é cada um ter a sua pasta e o seu ambiente (R28). Regra única: `git pull` antes de começar. **Uma exceção, e uma só** (R39): quem tomou por conta própria uma decisão que o grupo ainda não viu abre pull request, para que ela possa cair antes de entrar na `main` — foi o caso do portal do professor (#29). Revisão obrigatória continua fora: com uma placa só, ela somaria espera sem tirar conflito nenhum.

## 10. Principal desafio técnico

**Fazer o rádio único do ESP32 atender ao professor e à nuvem sem derrubar o portal, mantendo o registro de presença abaixo de 200 ms e o relatório íntegro quando a rede falhar.**

Quatro coisas disputam o mesmo chip: o SPI do RC522 com UIDs de dois tamanhos, o WebServer + DNS do portal cativo, a escrita append-only no LittleFS (que não pode corromper em queda de energia) e a janela de STA para falar com a API.

A pesquisa técnica confirmou o desafio e o tornou mais preciso, e uma correção do grupo em 10/09 (R19) mudou o formato dele. O ESP32 tem **uma antena só**, mas isso não impede hotspot e cliente ao mesmo tempo: o modo `WIFI_AP_STA` é nativo, e a restrição real é de **canal** — as duas interfaces ficam no canal do cliente, e o hotspot anuncia a migração por CSA, que celular moderno acompanha sem cair. O que sobra como impedimento é **memória**: o handshake TLS exige 40–50 KB de heap livre, e esse número precisa sobrar com hotspot, DNS, WebServer e roster carregados ao mesmo tempo.

O desafio, portanto, deixou de ser "como alternar" e virou **qual dos dois arranjos o hardware sustenta** — o simultâneo (Plano A), que nunca derruba o professor, ou a alternância (Plano B), que devolve memória ao preço de 5 a 15 segundos por transição e do risco conhecido do arduino-esp32 em que o hotspot não volta. A resposta é medida na bancada, nos testes 7c e 7d do §8.

Concentrar toda a rede em dois instantes (login e Enviar), em vez de uma chamada por aluno, é o que torna o problema tratável: reduz de ~40 janelas de risco por aula para 2. O preço é um modo offline degradado e uma recuperação de falha que precisam se comportar direito — porque quando a troca falha, o que está em jogo é uma aula inteira de presenças dentro do aparelho.

## 11. Glossário

**Tag** = crachá físico. **UID** = número de série do chip (4 ou 7 bytes). **Cadastro** = o vínculo entre o crachá e o aluno, feito pela equipe ou pela instituição, fora do aparelho (R1); o aparelho nunca cadastra ninguém. **Roster** = lista da turma (matrícula + nome + UID) baixada da API no login (R14). **Sessão** = janela aberta pelo professor. **Registro** = gravar a presença de um aluno na sessão; acontece uma vez por aluno por sessão (R30). **Evento** = uma linha do `eventos.json`: um toque aceito ou um lançamento manual. **Presença** = o aluno em `presentes`, calculada a partir dos eventos. **Duplicata** = defeito: o mesmo toque gravado mais de uma vez, que é o que o teste 8 procura. O toque de quem já está registrado não é duplicata: é `JA_REGISTRADO`, e nada é gravado. **Modo offline** = sessão cujo login aconteceu sem internet; grava só UIDs. **Enriquecimento** = juntar nome/matrícula ao UID. **Janela de silêncio** = os 5 s em que novas leituras do mesmo UID são ignoradas sem sinal, para o crachá parado no leitor (chamava-se *debounce* até a R30). **Chave publicável** = a chave do Supabase que vai no aparelho e na página do histórico (`sb_publishable_…`); sem login vale como papel `anon`, que só executa as duas funções do banco, e com login vale como `authenticated`, que só lê (R33–R35). **AP** = o aparelho como rede Wi-Fi. **STA** = o aparelho como cliente de outra rede. **AP+STA** = os dois ao mesmo tempo, no mesmo canal (Plano A, R19). **CSA** = o aviso de troca de canal que o AP manda às estações conectadas antes de migrar. **Plano A / Plano B** = modo simultâneo / alternância `AP → STA → AP`, a decidir pela bancada. **Portal do professor** = as páginas que o aparelho serve pela própria rede Wi-Fi e que abrem no navegador do celular; o aparelho não tem tela. **Painel** = a página do portal que abre depois do login, com as seis ações do §6; o painel do Supabase, onde a equipe administra o banco, é outra coisa. **Portal cativo** = o mecanismo que faz o celular abrir sozinho a página de login ao entrar na rede do aparelho, numa janelinha do sistema; é por ele que o professor chega ao portal sem digitar endereço.

## 12. Referências

OLIVEIRA & ANDRADE (2010) — hardware/firmware, barramentos SPI, temporização, drivers. GUIMARÃES (2009) — sensores e atuadores em sistemas embarcados. YAGHMOUR et al. (2009) — arquitetura de sistemas embarcados e integração (uso conceitual; o firmware aqui é bare-metal Arduino Core, não Linux embarcado).
