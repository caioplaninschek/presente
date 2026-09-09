# Esquema do Supabase e como a chave anon é restringida

Type: grilling
Status: open
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

## Notas

Bloqueado por **Caminho viável do ESP32 até o Supabase**: se a pesquisa mostrar que HTTPS direto é inviável e o plano B for um proxy, o desenho das tabelas continua válido mas o contrato de acesso muda inteiro. Decidir esquema antes de saber o transporte é retrabalho provável.

Tem componente de `domain-modeling` forte: os termos `roster`, `sessão`, `evento`, `enriquecimento` e `modo offline` já estão no glossário da spec (§11) e o esquema precisa respeitá-los, não inventar sinônimos.
