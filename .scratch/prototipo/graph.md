# Execução: do protótipo até 22/09

Criado: 2026-09-11
Origem: `/to-tickets` sobre o `docs/spec.md` e o mapa `.scratch/presente/map.md`, depois da rodada 7 fechar a fase de decisão.

## Por que este diretório existe, separado de `.scratch/presente/`

`.scratch/presente/` é o mapa do Wayfinder: tickets de **decisão**, onde `Status: resolved` quer dizer "a pergunta foi respondida". Aqui são tickets de **execução**, onde pronto quer dizer "o código roda e tem evidência". Dois sentidos no mesmo campo, no mesmo diretório, seriam ilegíveis daqui a um mês.

O mapa do Wayfinder continua dono de três fios abertos, e só: a metade de RLS do ticket 06, o veredito do ticket 09, e o ticket 08, que voltou a `reopened` com o R19. Cada um fecha quando o ticket de execução correspondente fechar (07, 18 e 18, nesta ordem).

## Inversão de convenção, declarada

O `map.md` registrou que as issues do GitHub são o fórum das decisões e o `.scratch` é o registro de trabalho. Para a execução isso se inverte, e de propósito.

As peças estão todas com o João, e o combinado é que ele receba o trabalho pronto para copiar e colar. Isso põe o enunciado rico — passo a passo, o que tem que aparecer, o que fazer se der errado — dentro da issue do GitHub, que é onde ele lê e onde ele responde com o log e a foto. Os arquivos daqui ficam finos de propósito: título, dono, prazo, aresta e ponteiro. Duplicar o texto nos dois lugares só produziria duas versões divergentes.

## O grafo

```
sem bloqueio, pode começar hoje
  01 fluxograma (Gabriel, 13/09) ─┐
  02 texto de evolução (Caio) ────┼─> 09 PDF da Entrega 04 (Caio, 14/09)
  03 checklist (Caio) ────────────┘
  06 telas do professor (Igor, 21/09)
  07 banco na nuvem (Caio, 17/09) ──> [epic 05/10: tela de histórico]
  05 firmware do rádio (Caio) ──> 10 roteiro (Caio, 17/09) ─┐
  04 ambiente do João (Caio+João, 14/09) ───────────────────┼─> 14 teste 7d (João, 19/09)
  08 protoboard (João, 16/09) ──┬─> 11 buzzer (João, 16/09) │        │
                                │                            │        v
                                ├─> 12 prova de vida ────┐   │   16 teste 7c (João, 20/09)
                                └─> 13 leitor lê crachá ─┴───┼────────┤
                                                             │        v
                                       15 verde+bip+debounce │   18 veredito (Caio, 21/09)
                                              │              │
                                              v              │
                                       17 não se perde ──────┴─> 19 dossiê (Gabriel, 22/09)

08 protoboard ──> [epic 29/09: estrutura física]
```

A aresta que manda: **04** (o ambiente na máquina do João) trava tudo que precisa da placa. É a tarefa zero.

A aresta que decide: **14** antes de **16**. O teste da memória precisa de um celular só e pode reprovar o Plano A sozinho; o do painel precisa de dois celulares no mesmo cômodo, que é o único item do projeto que exige gente na mesma sala.

A aresta escondida, encontrada em 11/09: **07 antes de 05**, só pela metade. O firmware do teste de rádio se **escreve** sem nada, mas para **medir** o heap ele precisa abrir um TLS de verdade — e para isso o `secrets.h` precisa de um `SUPABASE_URL` que exista. O projeto no Supabase só nasce na 07. Não são as tabelas nem as políticas de RLS que travam: é o host existir. As duas são do Caio e vencem no mesmo dia (17/09), então a ordem custa nada — **criar o projeto no Supabase primeiro**, e a medição da 14 sai contra o host de verdade, com a cadeia de certificados que o aparelho vai encontrar em produção. A alternativa (medir contra um host HTTPS qualquer) funciona, mas deixa uma ressalva pendurada no veredito da 18 para sempre.

## De-para

| Local | GitHub | Epic | Dono | Prazo |
|---|---|---|---|---|
| 01 | [#10](https://github.com/caioplaninschek/presente/issues/10) | #5 | Gabriel | 13/09 |
| 02 | [#11](https://github.com/caioplaninschek/presente/issues/11) | #5 | Caio | 13/09 |
| 03 | [#12](https://github.com/caioplaninschek/presente/issues/12) | #5 | Caio | 13/09 |
| 04 | [#14](https://github.com/caioplaninschek/presente/issues/14) | #6 | Caio + João | 14/09 |
| 05 | [#15](https://github.com/caioplaninschek/presente/issues/15) | #6 | Caio | 17/09 |
| 06 | [#20](https://github.com/caioplaninschek/presente/issues/20) | #6 | Igor | 21/09 |
| 07 | [#21](https://github.com/caioplaninschek/presente/issues/21) | #6 | Caio | 17/09 |
| 08 | [#22](https://github.com/caioplaninschek/presente/issues/22) | #7 | João | 16/09 |
| 09 | [#13](https://github.com/caioplaninschek/presente/issues/13) | #5 | Caio | 14/09 |
| 10 | [#16](https://github.com/caioplaninschek/presente/issues/16) | #6 | Caio | 17/09 |
| 11 | [#23](https://github.com/caioplaninschek/presente/issues/23) | #7 | João | 16/09 |
| 12 | [#24](https://github.com/caioplaninschek/presente/issues/24) | #7 | livre + João | 17/09 |
| 13 | [#25](https://github.com/caioplaninschek/presente/issues/25) | #7 | Cauã + João | 19/09 |
| 14 | [#17](https://github.com/caioplaninschek/presente/issues/17) | #6 | João | 19/09 |
| 15 | [#26](https://github.com/caioplaninschek/presente/issues/26) | #7 | Cauã + João | 21/09 |
| 16 | [#18](https://github.com/caioplaninschek/presente/issues/18) | #6 | João | 20/09 |
| 17 | [#27](https://github.com/caioplaninschek/presente/issues/27) | #7 | Cauã + João | 22/09 |
| 18 | [#19](https://github.com/caioplaninschek/presente/issues/19) | #6 | Caio | 21/09 |
| 19 | [#28](https://github.com/caioplaninschek/presente/issues/28) | #7 | Gabriel | 22/09 |

Epics no GitHub: [#5](https://github.com/caioplaninschek/presente/issues/5) Entrega 04 · [#6](https://github.com/caioplaninschek/presente/issues/6) 21/09 · [#7](https://github.com/caioplaninschek/presente/issues/7) 22/09 · [#8](https://github.com/caioplaninschek/presente/issues/8) 29/09 · [#9](https://github.com/caioplaninschek/presente/issues/9) 05/10.

## Não fragmentado ainda

Os epics #8 (estrutura física, 29/09) e #9 (dashboard e o aparelho falando com a nuvem, 05/10) ficam como issue única. A caixa depende do tamanho real da placa montada e o dashboard depende do banco com dados dentro; vértice escrito hoje precisaria ser reescrito. Graduam depois de 22/09, pela mesma regra que o `map.md` já usa para outubro e novembro.

## Riscos conhecidos, registrados na abertura

1. **Nove execuções físicas em onze dias, todas pelo João.** É o preço do modelo em que a placa não circula. Se empilhar, a saída é uma tarde na casa dele com mais gente, ou a placa circular por alguns dias.
2. **Sem aula presencial na terça 15/09.** Some o único encontro garantido da semana, e é dele que o teste 16 (7c) depende, por precisar de dois celulares juntos.
3. **O Igor ainda não é colaborador do repositório** — convite pendente desde 06/09. A issue #20 e o epic #9 estão sem assignee por causa disso.
4. **O Gabriel não se manifestou em nenhuma issue até 11/09.** Os dois vértices dele (01 e 19) foram escolhidos de propósito sem placa e sem código, e nenhum dos dois trava terceiro.
