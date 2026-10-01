# Issue tracker: Local Markdown

> **30/09/2026: tarefa de execução vive só na issue do GitHub.** O enunciado, o dono, o prazo, a conversa e o andamento ficam lá, e nenhuma tarefa nova ganha ticket local. Os tickets de `.scratch/integracao/` foram apagados nesse dia, porque repetiam as issues e as duas cópias se desencontravam; o `graph.md` de lá ficou como mapa de dependências. Os de `.scratch/prototipo/`, todos fechados, ficam como registro. Daqui em diante, o que este arquivo diz sobre tickets locais vale só para os mapas de decisão do `/wayfinder`, como o `.scratch/presente/`.
>
> **Neste repo:** a spec é `docs/spec.md`, e não `.scratch/<feature-slug>/spec.md`. ~~Os tickets de execução (`.scratch/prototipo/` até 22/09, `.scratch/integracao/` desde 23/09) são finos de propósito: o enunciado completo de cada um, e a conversa sobre ele, vivem na issue do GitHub.~~ *(30/09: ver o parágrafo acima)* Ver `AGENTS.md`, "Onde o trabalho vive".

Issues and specs for this repo live as markdown files in `.scratch/`.

## Conventions

- One feature per directory: `.scratch/<feature-slug>/`
- The spec is `docs/spec.md` — a single spec for the whole project, not one per feature directory
- Implementation issues are one file per ticket at `.scratch/<feature-slug>/issues/<NN>-<slug>.md`, numbered from `01`, never a single combined tickets file
- Triage state is recorded as a `Status:` line near the top of each issue file (see `triage-labels.md` for the role strings)
- Comments and conversation history live in the GitHub issue the ticket points at, not at the bottom of the file

## When a skill says "publish to the issue tracker"

~~Create a new file under `.scratch/<feature-slug>/` (creating the directory if needed).~~ *(30/09)* Tarefa de execução: abrir uma issue no GitHub (`gh issue create`), sem arquivo local. Só o mapa de decisão do `/wayfinder` continua em `.scratch/<effort>/`, como diz a seção "Wayfinding operations".

## When a skill says "fetch the relevant ticket"

Read the file at the referenced path. The user will normally pass the path or the issue number directly.

## Wayfinding operations

Used by `/wayfinder`. The **map** is a file with one **child** file per ticket.

- **Map**: `.scratch/<effort>/map.md` (the Notes / Decisions-so-far / Fog body).
- **Child ticket**: `.scratch/<effort>/issues/NN-<slug>.md`, numbered from `01`, with the question in the body. A `Type:` line records the ticket type (`research`/`prototype`/`grilling`/`task`); a `Status:` line records `claimed`/`resolved`.
- **Blocking**: a `Blocked by: NN, NN` line near the top. A ticket is unblocked when every file it lists is `resolved`.
- **Frontier**: scan `.scratch/<effort>/issues/` for files that are open, unblocked, and unclaimed; first by number wins.
- **Claim**: set `Status: claimed` and save before any work.
- **Resolve**: append the answer under an `## Answer` heading, set `Status: resolved`, then append a context pointer (gist + link) to the map's Decisions-so-far in `map.md`.
