# Domain Docs

> **Neste repo:** o glossário é o §11 de `docs/spec.md`. Não crie `CONTEXT.md` paralelo, porque dois glossários divergem; termo resolvido entra no §11. ADRs continuam em `docs/adr/`, sob demanda.

How the engineering skills should consume this repo's domain documentation when exploring the codebase.

## Before exploring, read these

- **`docs/spec.md`, §11** — the glossary of this repo. There is no `CONTEXT.md` and there will not be one: two glossaries diverge.
- **`docs/adr/`**: read ADRs that touch the area you're about to work in.

If `docs/adr/` doesn't exist yet, **proceed silently**. Don't flag its absence; don't suggest creating it upfront. A term that gets resolved goes into §11 of the spec, and a decision that gets closed becomes an `R<n>` there — see `AGENTS.md`, "Quando uma decisão fecha".

## File structure

Single-context repo (most repos):

```
/
├── CONTEXT.md
├── docs/adr/
│   ├── 0001-event-sourced-orders.md
│   └── 0002-postgres-for-write-model.md
└── src/
```

Multi-context repo (presence of `CONTEXT-MAP.md` at the root):

```
/
├── CONTEXT-MAP.md
├── docs/adr/                          ← system-wide decisions
└── src/
    ├── ordering/
    │   ├── CONTEXT.md
    │   └── docs/adr/                  ← context-specific decisions
    └── billing/
        ├── CONTEXT.md
        └── docs/adr/
```

## Use the glossary's vocabulary

When your output names a domain concept (in an issue title, a refactor proposal, a hypothesis, a test name), use the term as defined in §11 of `docs/spec.md`. Don't drift to synonyms the glossary explicitly avoids — it retired *debounce* in favour of *janela de silêncio*, for one.

If the concept you need isn't in the glossary yet, that's a signal: either you're inventing language the project doesn't use (reconsider) or there's a real gap (note it for `/domain-modeling`).

## Flag ADR conflicts

If your output contradicts an existing ADR, surface it explicitly rather than silently overriding:

> _Contradicts ADR-0007 (event-sourced orders), but worth reopening because…_
