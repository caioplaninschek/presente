# Triage Labels

The skills speak in terms of five canonical triage roles. This file maps those roles to the actual label strings used in this repo's issue tracker.

| Label in mattpocock/skills | Label in our tracker | Meaning                                  |
| -------------------------- | -------------------- | ---------------------------------------- |
| `needs-triage`             | `needs-triage`       | Maintainer needs to evaluate this issue  |
| `needs-info`               | `needs-info`         | Waiting on reporter for more information |
| `ready-for-agent`          | `ready-for-agent`    | Fully specified, ready for an AFK agent  |
| `ready-for-human`          | `ready-for-human`    | Requires human implementation            |
| `wontfix`                  | `wontfix`            | Will not be actioned                     |

When a skill mentions a role (e.g. "apply the AFK-ready triage label"), use the corresponding label string from this table.

Estados de fim, fora da tabela: ticket de **decisão** (`.scratch/presente/`) fecha em `resolved`, com a linha `Resolvido em:`; ticket de **execução** (`.scratch/prototipo/`) fecha em `done`, com a linha `Fechado em: DD/MM/AAAA, pela issue #N do GitHub`. Ticket de decisão também usa `partial` (metade resolvida, com uma linha dizendo o que falta), `reopened` (reaberto por decisão nova, com a data e a rodada), `deferred` (adiado, com a data e o motivo) e `in-progress` (em andamento, com dono).

Edit the right-hand column to match whatever vocabulary you actually use.
