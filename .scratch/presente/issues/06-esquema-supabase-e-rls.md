# Esquema do Supabase e como a chave anon é restringida

Type: grilling
Status: resolved
Resolvido: 13/09/2026, na preparação da issue #21 do GitHub (R33–R35)
Esquema decidido em: 10/09/2026, pela issue #1 do GitHub (R20) — `alunos`, `professores`, `turmas`, `alunos_turmas`, `sessoes` (com `estado`) e `eventos`. A idempotência sai do `estado`: sessão `finalizada` recusa o segundo POST, o que fecha `HU-39`. ~~Continua em aberto a outra metade do ticket: escrever as políticas de RLS (a chave anon lê roster e insere evento, nunca apaga nem altera o passado). Dono: Caio.~~ — a outra metade fechou em 13/09 com a **R33**, que tirou o aparelho das tabelas: ele não tem privilégio em nenhuma e só executa `roster` e `enviar_relatorio`. A chave também deixou de ser a `anon`: é a **publicável** (R35). Ver a `## Answer` abaixo.
Parent: map.md
Blocked by: 03

Discussao do grupo: https://github.com/caioplaninschek/presente/issues/1
Versao em linguagem simples, para debate com o grupo: docs/pauta-do-grupo.md — Parte 1, pontos 1 a 6.

## Question

A spec esboça quatro tabelas (`alunos`, `sessoes`, `eventos`, `alertas`) e diz "chave anon + RLS", mas isso é título, não esquema. O que falta decidir:

1. **Colunas e chaves** de cada tabela, e como o `GET /alunos?turma=eq.<turma>` do Iniciar se relaciona com o `POST /sessoes` + `POST /eventos` do Enviar.
2. **O problema de fundo da chave anon:** ela vai estar dentro do binário do ESP32, num aparelho fixo numa sala de aula, sem vigilância. Quem extrair a chave consegue o que a RLS permitir. Qual é a política — a chave pode ler a lista de alunos de qualquer turma? Pode inserir eventos em qualquer sessão? Pode apagar?
3. **Idempotência do envio.** Se o professor clicar Enviar duas vezes, ou o retry disparar depois de um sucesso parcial, o relatório não pode duplicar. Isso é chave única no banco, `upsert`, ou controle no firmware?
4. **Enriquecimento no modo offline (R3).** Quando o relatório chega só com UIDs, quem faz o de-para para matrícula: uma view, uma function no Postgres, ou o cliente que consome? A spec diz "do lado da API" sem dizer onde.

## Answer

Respondida em 13/09/2026, na preparação da issue #21. O detalhe está na spec, em R33, R34 e R35, e o SQL em `supabase/001-esquema.sql`.

1. **Colunas e chaves.** As seis tabelas da R20 levam as colunas do contrato do §7 com nome de banco: `sessoes.id` é o `sessaoId`, e `eventos.horario` é o `timestamp`. O aparelho não usa endpoint de tabela; no login chama `roster(turma)`, e no Enviar chama `enviar_relatorio(relatorio)` com o JSON do Compartilhar.
2. **O que a chave pode.** Nada nas tabelas: ela só executa as duas funções. Lê a lista de uma turma cujo código conheça, não lê a lista de turmas nem o histórico, e não apaga nem altera nada, porque a tentativa dá erro de permissão. O histórico se lê com login (R34), e a chave é a publicável (R35).
3. **Idempotência.** No banco, pelo `sessaoId`: a sessão chega já `finalizada`, e, se o id existe, a função responde `ja_enviado` sem gravar nada. O `sessaoId` passou a ser `<turma>-<AAAAMMDD>-<HHMM>` para não colidir entre turmas.
4. **Enriquecimento offline.** Dentro de `enviar_relatorio`, na chegada: evento com UID e sem matrícula recebe a matrícula do dono do UID, e `enriquecido` guarda o que o aparelho mandou.

## Notas

Bloqueado por **Caminho viável do ESP32 até o Supabase**: se a pesquisa mostrar que HTTPS direto é inviável e o plano B for um proxy, o desenho das tabelas continua válido mas o contrato de acesso muda inteiro. Decidir esquema antes de saber o transporte é retrabalho provável.

Tem componente de `domain-modeling` forte: os termos `roster`, `sessão`, `evento`, `enriquecimento` e `modo offline` já estão no glossário da spec (§11) e o esquema precisa respeitá-los, não inventar sinônimos.
