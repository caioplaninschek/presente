# Comportamento do rádio: quando sincronizar e o que acontece quando a troca falha

Type: grilling
Status: open
Parent: map.md
Blocked by: —

## Question

A pesquisa **Caminho viável do ESP32 até o Supabase** confirmou que a alternância AP→STA→AP funciona, mas é **o ponto de falha mais provável do projeto**, com três bugs conhecidos no arduino-esp32 e **5–15 s por transição** — não os ~3 s que a spec estimava em R4.

Isso abre duas decisões de produto que ninguém tomou ainda.

### 1. Quando sincronizar?

O fluxo canônico (§6) manda buscar o roster **quando o professor clica Iniciar**. Com 5–15 s de transição, o professor clica e fica olhando para a tela até um terço de minuto, no começo da aula, com a turma esperando na porta.

Alternativas:

- **Manter no Iniciar.** Simples, previsível, mas cobra a espera no pior momento.
- **Antecipar para o login.** O aparelho busca o roster enquanto o professor digita usuário e senha; quando ele chega ao painel, a lista já está lá. Esconde a espera atrás de trabalho que o humano já estava fazendo.
- **Antecipar para o boot.** Ainda mais cedo, mas o aparelho não sabe qual turma buscar antes de saber qual professor logou.

### 2. O que acontece quando a troca falha?

Cenário concreto: o professor clica Enviar no fim da aula, o rádio vai para STA, e o AP não volta (bug #4294 / esp32.com t=37829). O professor perde o portal com 40 presenças registradas dentro do aparelho.

- O aparelho reinicia sozinho (`ESP.restart()`) e volta ao AP? Se sim, **a sessão sobrevive ao reboot?** Hoje a spec garante que os *eventos* persistem em LittleFS, mas não diz nada sobre o **estado da sessão** (aberta/fechada, `professorId`, roster em cache).
- Ou o aparelho tenta de novo N vezes antes de reiniciar? Quantas? Lembrar que cada `connect()` falho vaza ~4 KB de heap.
- E o LED — o professor precisa distinguir "sincronizando" de "travado"? Hoje os dois seriam azul piscando.

### 3. Decorrência: a sessão precisa ser persistida?

Se a resposta de (2) for "reinicia", então `sessao_atual.json` deixa de ser conveniência e vira **requisito**: gravado antes de toda troca de rádio, lido no boot para retomar a sessão em curso em vez de cair na tela de login.

## Notas

`grilling` porque as três perguntas são de produto (o que o professor vê e o que ele perde), não de implementação. A pesquisa já entregou os fatos técnicos; falta decidir o comportamento.

Enviesado para: sincronizar no login, `ESP.restart()` após 2 tentativas falhas, e `sessao_atual.json` como requisito. Mas a decisão é do Caio — especialmente a primeira, que muda o fluxo de 16 passos já escrito na spec.
