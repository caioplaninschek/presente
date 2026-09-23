# O dashboard em nuvem existe, ou o portal do ESP32 basta?

Type: grilling
Status: resolved
Resolvido em: 10/09/2026, pela issue #2 do GitHub (R21) — a página existe, na opção B (histórico simples lendo o banco, sem tocar no firmware), e o dono é o Igor. Fecha `HU-30`.
Parent: map.md
Blocked by: 03
Prazo real: ~~05/10~~ cerca de uma semana depois, a confirmar no enunciado (entregável "Dashboard"; em 22/09 o professor avisou que as entregas depois de 29/09 andam uma semana)

Discussao do grupo: https://github.com/caioplaninschek/presente/issues/2
Versao em linguagem simples, para debate com o grupo: docs/pauta-do-grupo.md — Parte 2, ponto 7.

## Question

O professor exige, em 05/10: *"Dashboard (ESP32 obrigatório) ou interface Serial/Processing"*. O portal servido pelo ESP32 **já cumpre o requisito inteiro** — presentes vs. faltantes em tempo real, controles, exportação.

Então a pergunta é de escopo, não de nota: **vale construir uma segunda interface, em nuvem, que mostre o histórico acumulado de várias aulas?**

O que pesa de cada lado:

- **A favor.** É o que transforma "um leitor de crachá" em "um sistema": na apresentação de 27/10, mostrar 8 aulas acumuladas com presença por aluno tem peso narrativo que uma tela de aula única não tem. E o Supabase já vai existir por causa do roster.
- **Contra.** É uma frente de front-end inteira, num time de 5 que já tem firmware, portal, hardware, caixa, vídeo e relatório. E não vale um ponto de nota a mais no 05/10.

Opções a confrontar:

1. **Nada.** O painel administrativo nativo do Supabase é a "visão em nuvem", mostrado como está. Custo zero.
2. **Uma página estática** (HTML + JS, hospedada de graça) que lê o Supabase e desenha a tabela histórica. Custo baixo, ganho narrativo real.
3. **Dashboard completo** com gráficos, filtros por turma e período. Custo alto, ganho marginal sobre a opção 2.

## Notas

Bloqueado por **Caminho viável do ESP32 até o Supabase**: se o plano B mudar onde os dados param, muda o que um dashboard leria.

Enviesado para a opção 2. Ela compra quase toda a narrativa da opção 3 por uma fração do trabalho, e pode ser feita por quem não mexe em firmware — o que a torna paralelizável de verdade.
