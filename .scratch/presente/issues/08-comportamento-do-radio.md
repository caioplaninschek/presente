# Comportamento do rádio: quando sincronizar e o que acontece quando a troca falha

Type: grilling
Status: resolved
Resolvido de novo em: 22/09/2026, pela issue #19 do GitHub (R45) — vale o Plano A. A resposta 1 (sincronizar no login) continua valendo, sem a troca de rádio. A resposta 2 (duas tentativas e `ESP.restart()` quando o AP não volta) caiu, porque no Plano A o AP nunca é desligado. A resposta 3 (`sessao_atual.json` como requisito) continua, por causa da queda de energia.
Reaberto em: 10/09/2026, pela issue #3 do GitHub (R19) — a premissa "AP e STA nunca simultâneos" (R4) está errada: `WIFI_AP_STA` é nativo. O ticket continua válido em tudo que trata de *quando* sincronizar e do que fazer quando a rede falha; o que caiu foi a suposição de que a alternância é o único arranjo possível. A escolha entre Plano A (simultâneo) e Plano B (alternância) é medida nos testes 7c e 7d do §8, dentro do ticket 09.
Parent: map.md
Blocked by: —
Resolvido: 2026-09-06

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

## Answer

### 1. Sincronizar no login

O aparelho valida a senha, responde ao navegador *"carregando turma…"*, e **só então** troca o rádio para buscar o roster. Quando o painel abre, a lista já está em cache e o botão **Iniciar** é instantâneo.

A espera de 5–15 s não desaparece — ela **muda de lugar**, para o instante em que o professor acabou de submeter um formulário e já espera que a página demore. Sair de "botão que deveria ser instantâneo trava 15 segundos" para "login demora um pouco" é ganho de percepção, não de engenharia.

**Terceira opção que foi levantada e descartada:** tirar a internet do ESP32 e deixar o celular do professor falar com o Supabase (o aparelho nunca sairia do modo AP). É tecnicamente superior — mataria a troca de rádio, o bug do AP, o TLS e 40–50 KB de heap. Descartada por razão **acadêmica, não técnica**: numa disciplina de Sistemas Embarcados, fazer o ESP32 falar com a nuvem é justamente a competência que está sendo avaliada. Vale registrar essa alternativa no relatório técnico como caminho de evolução — mostra que a escolha foi consciente.

### 2. Recuperação quando o rádio não volta

- **Duas tentativas** de restabelecer o AP. Não mais: cada `connect()` falho vaza ~4 KB de heap (R13b), então retry infinito derruba o aparelho de qualquer jeito.
- Falhando as duas, **`ESP.restart()`**. Em sala, um reboot de 2 s é aceitável; um portal que não volta, não.
- **O LED precisa distinguir três coisas**, senão o professor não sabe se espera ou se bate no aparelho:
  - azul piscando **lento** = sincronizando, aguarde
  - azul piscando **rápido** = falhou, vou reiniciar
  - azul fixo = de pé, aguardando login

### 3. `sessao_atual.json` vira requisito

Deixa de ser conveniência. Gravado **antes de toda troca de rádio**, lido no boot. Se o aparelho reiniciar no meio da aula, ele volta direto para a sessão em curso — mesmo `professorId`, mesmo roster, mesma janela aberta — em vez de cair na tela de login com 40 presenças órfãs dentro.

Este é o item que transforma o reinício de "perda de aula" em "soluço de 2 segundos", e é o que torna a decisão (2) aceitável.
