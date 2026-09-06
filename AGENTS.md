# Presente!

Terminal embarcado (ESP32 + RC522 + SW-420) para contabilização de presença em sala de aula. Trabalho da disciplina Sistemas Embarcados — UVA Barra, 3ª terça-feira.

A especificação do projeto está em `docs/spec.md`. Leia antes de propor qualquer mudança de arquitetura, contrato JSON ou pinagem: as decisões Q1–Q9, R1–R8 e R9–R12 já foram fechadas.

O mapa de decisões em aberto está em `.scratch/presente/map.md`. Leia o mapa antes de trabalhar um ticket.

Nunca assine commits como IA: sem `Co-Authored-By` de assistente, sem `🤖 Generated with`, sem trailer equivalente.

## Diário de bordo

O professor exige evidência de evolução semanal. Duas camadas, e as duas valem nota:

- **O histórico do git é a prova primária.** Commits datados, mensagens em português descrevendo o que mudou de verdade. Não amontoar uma semana de trabalho num commit só.
- **`DIARIO.md` na raiz é a versão legível.** Uma entrada por semana com data, o que foi feito, o que travou e o que vem a seguir. Atualizar **toda semana**, antes da entrega da terça.

Toda sessão de agente que produzir trabalho substantivo neste repo deve deixar a entrada correspondente no `DIARIO.md`.

## Credenciais

Chave do Supabase e SSID/senha do hotspot vivem em `secrets.h`, que está no `.gitignore`. O que é versionado é `secrets.example.h`, com os campos em branco. Nunca commitar credencial real, nem em exemplo, nem em comentário, nem no relatório.

## Agent skills

### Issue tracker

Issues e specs vivem como markdown em `.scratch/<feature>/` neste repo. See `docs/agents/issue-tracker.md`.

### Triage labels

Vocabulário canônico padrão (`needs-triage`, `needs-info`, `ready-for-agent`, `ready-for-human`, `wontfix`), gravado como linha `Status:` no topo de cada arquivo de issue. See `docs/agents/triage-labels.md`.

### Domain docs

Single-context: um `CONTEXT.md` na raiz e ADRs em `docs/adr/`, criados sob demanda. See `docs/agents/domain.md`.
