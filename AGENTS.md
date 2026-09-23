# Presente!

Terminal embarcado (ESP32 + RC522) para contabilização de presença em sala de aula. Trabalho da disciplina Sistemas Embarcados — UVA Barra, 3ª terça-feira.

A especificação do projeto está em `docs/spec.md`. Leia antes de propor qualquer mudança de arquitetura, contrato JSON ou pinagem. **Quantas rodadas já fecharam, leia no cabeçalho da própria spec** — não confie em contagem escrita fora dela, aqui inclusive.

O mapa de decisões em aberto está em `.scratch/presente/map.md`. Leia o mapa antes de trabalhar um ticket.

Os enunciados do professor, com o detalhamento da próxima entrega, estão em `docs/entregas.md`. **O prazo de cada entrega é o do enunciado.** Data que ainda não saiu de enunciado é provisória e se confere quando ele for publicado; não deduza prazo de dia da semana nem de turma.

**Todo texto que sai deste projeto segue `docs/padrao-editorial.md`** — PDFs de entrega, relatório técnico, `DIARIO.md`, roteiro de vídeo. Leia antes de escrever qualquer documento.

**Commit direto na `main` é o padrão**, sem branch nem revisão obrigatória (§9). A exceção é uma só, e tem razão fixa: quem tomou por conta própria uma decisão que o grupo ainda não viu abre pull request, para que ela possa cair **antes** de entrar na `main` (R39).

Nunca assine commits como IA: sem `Co-Authored-By` de assistente, sem `🤖 Generated with`, sem trailer equivalente.

## Onde o trabalho vive

São três lugares, com papéis diferentes. Confundi-los produz duas versões divergentes da mesma coisa.

- **Issues do GitHub** — o fórum do grupo e, desde 11/09, **o enunciado completo de cada tarefa de execução**: passo a passo de bancada, o que tem que aparecer, o que fazer se der errado. É onde os integrantes leem e respondem com log e foto.
- **`.scratch/presente/`** — tickets de **decisão**, indexados pelo `map.md`. `Status: resolved` quer dizer "a pergunta foi respondida".
- **`.scratch/prototipo/`** — tickets de **execução**, indexados pelo `graph.md`. Pronto quer dizer "o código roda e tem evidência", e o ticket pronto vira `Status: done`, com a data e a issue que o fecharam. São finos de propósito: título, dono, prazo, aresta, `HU-xx` e ponteiro para a issue — o texto rico vive lá, não aqui.
- **`.scratch/integracao/`**: tickets de execução do plano de 23/09 até a entrega final, indexados pelo `graph.md` de lá, no mesmo formato fino.

**Como se pega uma issue (R52).** Atribua a issue a você e comente "peguei" antes de começar. A linha *Requer* diz o que a issue exige, e só as de dono natural já nascem atribuídas. Ninguém designa tarefa a ninguém.

**Todo ticket de implementação cita a história de usuário que justifica o seu critério** (`HU-xx`, §1.1 da spec). O §1.1 declara as histórias como a referência citável dos critérios de aceitação; sem a citação, as 40 histórias viram texto que ninguém abre e o critério perde a razão de ser o que é. Ticket de documentação, logística ou montagem física pode não ter `HU` — nesse caso, não invente uma.

## Quando uma decisão fecha

Decisão fechada numa issue **não termina na `spec.md`**. Em 11/09 uma conferência da spec contra as tarefas achou três decisões vivas na prática e mortas no documento — e a pior delas seguia sendo **oferecida ao grupo na pauta** depois de já ter sido derrubada. A ordem é sempre esta, e nenhum passo é opcional:

1. **`docs/spec.md`** — a decisão vira um `R<n>` em rodada datada do §2, e as seções que ela altera são corrigidas na mesma passada.
2. **Varra quem repete a decisão em outra linguagem, sem confiar na memória.** Escolha termos fixos para a decisão e busque-os em **todo arquivo de texto versionado** (`git grep -i -E`) **e em todo corpo e comentário de issue** (`gh issue list --state all --json number,body,comments`). Cada ocorrência recebe um veredito, e só se termina quando nenhuma ficar sem. O trecho **muda** se: (a) afirma algo que a decisão tornou falso e quem ler vai agir errado; (b) oferece como viva uma opção derrubada; (c) descreve um procedimento que ela mudou; (d) é índice, ponteiro ou contagem que deixa de achar a coisa nova. **Não muda** documento já entregue (passo 4), ticket de decisão resolvido, material de fornecedor nem palavra usada noutro sentido. **Corpo de issue muda; comentário de issue nunca**, porque é registro datado. Os suspeitos de sempre continuam sendo o ponto de partida, não a fronteira: `docs/pauta-do-grupo.md` (**o mais urgente, porque é o texto voltado para o grupo**), `.scratch/presente/map.md`, o ticket de decisão correspondente, os tickets de execução que dependiam dela e as issues abertas. **Comentário de código é texto versionado como qualquer outro**, e o `git grep` do passo tem que alcançá-lo: quando a decisão for de firmware, `lib/` e `src/` entram na lista de suspeitos junto com os documentos. A varredura da rodada 12 leu só `.md` e deixou onze resíduos vivos no repositório, achados no pente-fino de 16/09 — comentário que explica o porquê de uma decisão envelhece igual a documento, e é lido por quem está com o código aberto.
3. **Opção derrubada se risca com a razão e a data, nunca se apaga.** Quem a propuser de novo precisa esbarrar no motivo de ela ter caído.
4. **Documento já entregue ao professor não se reescreve.** A divergência se **declara** ao pé dele — foi assim com o controle de saída (§1), com o segundo sensor (§4) e com a divisão de equipe (§9). Reescrever o que já foi lido produz exatamente a incoerência que se quer evitar.
5. **`DIARIO.md`** — se a decisão muda o que o professor vê, entra na entrada da semana corrente.

**Não enumere rodadas fora da spec.** Ponteiro do tipo "R1 a R18 já estão fechadas" envelhece sozinho e em silêncio: foi assim que este arquivo e o `map.md` passaram uma semana desatualizados. Aponte para o cabeçalho da `spec.md`, que é mantido a cada rodada.

**Lacuna conhecida se declara.** Requisito da spec que ainda não tem ticket entra no §8 como lacuna, com a data em que vira tarefa. Ausência declarada é planejamento; ausência silenciosa é descuido, e o professor lê a diferença.

## Diário de bordo

O professor exige evidência de evolução semanal. São **duas camadas com cadências diferentes** — não as misture:

- **Histórico do git — cadência de sessão.** Commits datados, mensagem em português dizendo o que mudou de verdade. Não amontoar uma semana num commit só. É a prova primária e não precisa de curadoria.
- **`DIARIO.md` na raiz — cadência semanal.** Uma entrada por semana, fechada antes da entrega de terça. É o que o professor efetivamente lê.

**Regra para agentes:** ao produzir trabalho substantivo, **edite a entrada da semana corrente** — crie-a se ainda não existir, senão acrescente à que já está lá. Nunca abra uma entrada nova por sessão: o diário é semanal, não um segundo log de commits.

**Escreva para o professor, não para desenvolvedores.** O que foi feito, o que travou, o que vem a seguir. Sem nome de arquivo, sem nome de função, sem hash de commit — isso tudo já está no git, que é a outra camada.

**Ao fechar a semana, dê uma passada de narrativa na entrada inteira.** Durante a semana os blocos entram na ordem em que o trabalho acontece, que é a ordem dos commits; no fechamento eles se reagrupam **por frente**, para o professor ler uma história por assunto em vez de um extrato cronológico. A passada é a última coisa antes da entrega de terça, e não se antecipa: blocos ainda em movimento se reescreveriam duas vezes.

A entrada é em **terceira pessoa, nomeando quem fez** (`docs/padrao-editorial.md`).

## Firmware

Programa novo ganha pasta própria em `src/<nome>/` e ambiente próprio no `platformio.ini`; nunca defina `default_envs`. **Código que dois programas dividirem sobe para `lib/<módulo>/`**, com o nome do módulo do §5 que ele vai virar (`rfid`, `feedback`, `storage`…): tudo que está em `lib/` fica visível a todos os ambientes, sem configuração. Nenhuma biblioteca entra sem versão fixada. O porquê está na spec (R28, R29).

A partição de LittleFS é **uma só**, dividida entre o portal (`data/`) e o `eventos.json`. Por isso o `platformio.ini` declara `board_build.filesystem = littlefs`, e o `LittleFS.begin(true)` — que formata quando a montagem falha — vale nos programas de bancada e sai no firmware integrado (R41).

## Portal do professor

O HTML, o CSS e o JavaScript do portal moram em **`data/`** na raiz, que é a pasta de onde o PlatformIO gera a imagem de filesystem (R27). É uma página só, sem biblioteca e sem recurso externo (R10), e credencial nenhuma viaja no endereço (R25). Nenhuma tela avança sem o aparelho confirmar: todo pedido tem tempo-limite, com falha visível e o botão liberado (R40).

Enquanto o firmware não serve as páginas, `data/mentira.js` faz o papel do aparelho. As rotas `/api/*` que ele responde são **provisórias** e não são contrato — o contrato fecha no `portal.cpp`, e está declarado como lacuna no §8 da spec. Desde 23/09, o contrato se escreve no §6 da spec (tarefa 5.2, #36), e, quando o portal for ligado ao aparelho (tarefa 5.3, #37), o `mentira.js` passa a carregar só com `?mentira=1` no endereço (R51).

## Banco

O SQL do Supabase mora em `supabase/`, em arquivos numerados e aplicados pelo editor de SQL do painel. **Arquivo já aplicado não se edita:** mudança no banco vira o arquivo seguinte (`003-…`). Depois de aplicar qualquer um, rode `supabase/verificar.ps1`, que testa o banco com a chave do aparelho. O porquê está na spec (R33, R34, R35).

## Credenciais

Chave e URL do Supabase, SSID e senha do hotspot e SSID e senha da rede do próprio aparelho vivem em `secrets.h`, que está no `.gitignore`. O que é versionado é `secrets.example.h`, com os campos em branco. Nunca commitar credencial real, nem em exemplo, nem em comentário, nem no relatório.

## Agent skills

### Issue tracker

Os tickets vivem como markdown em `.scratch/<feature>/`, mas a spec é `docs/spec.md`, e o enunciado das tarefas de execução vive nas Issues do GitHub (ver "Onde o trabalho vive"). See `docs/agents/issue-tracker.md`.

### Triage labels

Vocabulário canônico padrão (`needs-triage`, `needs-info`, `ready-for-agent`, `ready-for-human`, `wontfix`), gravado como linha `Status:` no topo de cada arquivo de issue; os estados de fim e os de ticket de decisão também estão lá. See `docs/agents/triage-labels.md`.

### Domain docs

O glossário é o §11 da `docs/spec.md`: não crie `CONTEXT.md` paralelo, porque dois glossários divergem. ADRs em `docs/adr/`, criados sob demanda. See `docs/agents/domain.md`.
