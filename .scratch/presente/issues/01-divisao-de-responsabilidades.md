# Divisão de responsabilidades confirmada com o grupo

Type: task
Status: resolved
Parent: map.md
Resolvido: 2026-09-06
Blocked by: —
Prazo real: 07/09 20:00 (a Entrega 03 exige esta seção)

## Question

A seção 9 da spec traz uma **proposta** de divisão dos 5 papéis (Hardware · Firmware · Portal · Rede+API · Estrutura física e documentação) com um dono cada. Ela foi escrita pela IA, não pelo grupo — e o texto original que o grupo tinha discutido se perdeu junto com um chat antigo.

Quem, de fato, assume cada papel? A confirmação precisa vir dos 5, não do Caio sozinho, porque ela decide o dono de cada frente pelos próximos 3 meses e é a base de quase todo ticket seguinte deste mapa.

Sub-perguntas que a conversa com o grupo tem que fechar:

1. Cada um aceita o papel proposto, ou alguém quer trocar?
2. Quem é o revisor de cada papel (a proposta é revisão cruzada em cadeia)?
3. Alguém está sobrecarregado com outras disciplinas neste período e precisa de papel mais leve?

## Notas

Este ticket é `task` e não `grilling` porque não há nada a decidir *aqui*: o que bloqueia é o Caio levar a proposta ao grupo e voltar com a resposta. É o ticket mais urgente do mapa — a Entrega 03 de 07/09 exige "Divisão das responsabilidades da equipe" e "Cronograma da equipe até 21/09" como seções obrigatórias.

Resolvido quando os 5 nomes estiverem confirmados e a seção 9 da spec deixar de estar marcada como proposta.

## Answer

**O ticket estava mal formulado.** O Caio delegou a decisão e explicou por quê: *"na prática o que está no papel não necessariamente será a realidade — eu posso falar que outra pessoa está responsável e eu mesmo fazer"*.

Isso reclassifica o artefato. A divisão **não é um contrato operacional**; é uma seção obrigatória da Entrega 03. Ela precisa ser plausível e defensável diante do professor, não vinculante entre os 5. Buscar confirmação formal do grupo para algo que ninguém vai tratar como vinculante era cerimônia.

**Divisão travada** (spec §9, sai de "proposta"):

| Papel | Dono |
|---|---|
| Hardware | Gabriel |
| Firmware núcleo | Cauã |
| Portal web | Igor |
| **Integração e documentação** | **Caio** |
| Estrutura física e diário de bordo | João |

O papel do Caio é **amplo por desenho**, e isso é deliberado: "integração" legitima circular por qualquer frente. Quando ele fizer o trabalho de outro — e vai — o documento não o contradiz.

⚠️ **Superado em 11/09, pelo R23.** A tabela acima continua valendo como o que foi **entregue** na Entrega 03, e a resposta deste ticket ("documentação, não contrato") se provou certa — mas ela deixou de descrever quem faz o quê. Como há um microcontrolador só, **a placa parou de circular**: fica com o João, e toda task física nasce partida em *preparar* (qualquer um) e *rodar* (ele). A divisão operacional que vale hoje são as issues #5 a #28, e a divergência está declarada ao pé da tabela do §9 da spec.

**Decorrência que fecha outro ticket:** se na prática uma ou duas pessoas escrevem o código, o fluxo de git não precisa de branch nem PR. Ver *Quem escreve firmware, e como o time evita conflito no repo*.

**O que continua valendo:** o Caio ainda deve mandar a tabela no grupo — não para pedir autorização, mas para que ninguém seja pego de surpresa vendo o próprio nome num papel no PDF entregue.

⚠️ **23/09: a R52 tornou o trabalho livre.** Cada integrante pega as issues que quiser, e só as que dependem de acesso ou de trabalho em andamento nascem com dono. A tabela continua como registro do que foi entregue.
