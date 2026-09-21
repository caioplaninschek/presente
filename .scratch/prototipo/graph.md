# Execução: do protótipo até 22/09

Criado: 2026-09-11
Origem: `/to-tickets` sobre o `docs/spec.md` e o mapa `.scratch/presente/map.md`, depois da rodada 7 fechar a fase de decisão.

## Por que este diretório existe, separado de `.scratch/presente/`

`.scratch/presente/` é o mapa do Wayfinder: tickets de **decisão**, onde `Status: resolved` quer dizer "a pergunta foi respondida". Aqui são tickets de **execução**, onde pronto quer dizer "o código roda e tem evidência", e o ticket pronto recebe `Status: done` e uma linha `Fechado em: DD/MM/AAAA, pela issue #N do GitHub`. Dois sentidos no mesmo campo, no mesmo diretório, seriam ilegíveis daqui a um mês.

O mapa do Wayfinder continua dono de dois fios abertos, e só: o veredito do ticket 09 e o ticket 08, que voltou a `reopened` com o R19. Os dois fecham quando o ticket de execução 18 fechar. O terceiro fio, a metade de RLS do ticket 06, fechou com o 07 (R33–R35).

## Inversão de convenção, declarada

O `map.md` registrou que as issues do GitHub são o fórum das decisões e o `.scratch` é o registro de trabalho. Para a execução isso se inverte, e de propósito.

As peças estão todas com o João, e o combinado é que o trabalho chegue a ele pronto no repositório: o código na pasta do programa, o ambiente nomeado e o passo a passo na issue. Ele não copia código; ele puxa, escolhe o ambiente e grava. Isso põe o enunciado rico — passo a passo, o que tem que aparecer, o que fazer se der errado — dentro da issue do GitHub, que é onde ele lê e onde ele responde com o log e a foto. Os arquivos daqui ficam finos de propósito: título, dono, prazo, aresta, a história de usuário que justifica o critério (`HU-xx`, §1.1 da spec) e ponteiro. Duplicar o texto nos dois lugares só produziria duas versões divergentes.

## O grafo

```
sem bloqueio, pode começar hoje
  01 fluxograma (Gabriel, 13/09) ─┐
  02 texto de evolução (Caio) ────┼─> 09 PDF da Entrega 04 (Caio, 14/09)
  03 checklist (Caio) ────────────┘
  06 telas do professor (Igor, 21/09)
  07 banco na nuvem (Caio, 17/09) ──> [epic 05/10: tela de histórico]
  05 firmware do rádio (Caio) ──> 10 roteiro (Caio, 17/09)  ─┐
  04 ambiente do João (Caio+João, 14/09) ────────────────────┼─> 14 teste 7d (João, 19/09)
  08 protoboard (João, 19/09) ──┬─> 11 buzzer (João, 19/09)  │        │
                                │                            │        v
                                ├─> 12 prova de vida ────┐   │   16 teste 7c (João, 20/09)
                                └─> 13 leitor lê crachá ─┴───┼────────┤
                                                             │        v
                                       15 verde+bip uma vez  │   18 veredito (Caio, 21/09)
                                              │              │
                                              v              │
                                       17 não se perde ──────┴─> 19 dossiê (Gabriel, 21/09)

08 protoboard ──> [epic 29/09: estrutura física]
19 dossiê ──> 20 documento da Entrega 05 (Caio, 22/09 23:59)   (vértice acrescentado em 20/09, quando o enunciado saiu)
07 banco na nuvem ┄┄> 05 firmware do rádio   (aresta escondida: só o host do Supabase existir)
```

A aresta que manda: **04** (o ambiente na máquina do João) trava tudo que precisa da placa. É a tarefa zero.

A aresta que decide: **14** antes de **16**. O teste da memória precisa de um celular só e pode reprovar o Plano A sozinho; o do painel precisa de dois celulares no mesmo cômodo, que é o único item do projeto que exige gente na mesma sala.

A aresta escondida, encontrada em 11/09: **07 antes de 05**, só pela metade. O firmware do teste de rádio se **escreve** sem nada, mas para **medir** o heap ele precisa abrir um TLS de verdade — e para isso o `secrets.h` precisa de um `SUPABASE_URL` que exista. O projeto no Supabase só nasce na 07. Não são as tabelas nem as políticas de RLS que travam: é o host existir. As duas são do Caio e vencem no mesmo dia (17/09), então a ordem custa nada — **criar o projeto no Supabase primeiro**, e a medição da 14 sai contra o host de verdade, com a cadeia de certificados que o aparelho vai encontrar em produção. A alternativa (medir contra um host HTTPS qualquer) funciona, mas deixa uma ressalva pendurada no veredito da 18 para sempre.

~~A tarde que junta, decidida em 13/09: **domingo, 20/09.** O enunciado da Entrega 05 ainda não saiu, e os dois enunciados já publicados citam 21/09. Para tudo caber nessa data, o 15 e o 17 rodam na mesma tarde do 16, que já exige gente junta por causa dos dois celulares, com os programas prontos até sábado, 19/09; o 19 cai para segunda, 21/09. Se o enunciado disser 22/09, sobra um dia de folga.~~ A tarde de domingo não aconteceu, e a carga empilhou toda na segunda, 21/09. O enunciado saiu em 20/09 e fixou o envio em **22/09, às 23:59**, com demonstração presencial na aula do mesmo dia; é esse o dia de folga que a linha riscada previa, e ele virou o prazo (20/09).

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
| 08 | [#22](https://github.com/caioplaninschek/presente/issues/22) | #7 | João | ~~16/09~~ 19/09, o leitor depois da solda |
| 09 | [#13](https://github.com/caioplaninschek/presente/issues/13) | #5 | Caio | 14/09 |
| 10 | [#16](https://github.com/caioplaninschek/presente/issues/16) | #6 | Caio | 17/09 |
| 11 | [#23](https://github.com/caioplaninschek/presente/issues/23) | #7 | João | ~~16/09~~ 19/09, antes do 12 |
| 12 | [#24](https://github.com/caioplaninschek/presente/issues/24) | #7 | Cauã + João | ~~17/09~~ 19/09, assim que o 04 funcionar |
| 13 | [#25](https://github.com/caioplaninschek/presente/issues/25) | #7 | Cauã + João | 19/09, depois da solda |
| 14 | [#17](https://github.com/caioplaninschek/presente/issues/17) | #6 | João | 19/09 |
| 15 | [#26](https://github.com/caioplaninschek/presente/issues/26) | #7 | Cauã + João | 20/09 |
| 16 | [#18](https://github.com/caioplaninschek/presente/issues/18) | #6 | João | 20/09 |
| 17 | [#27](https://github.com/caioplaninschek/presente/issues/27) | #7 | Cauã + João | 20/09 |
| 18 | [#19](https://github.com/caioplaninschek/presente/issues/19) | #6 | Caio | 21/09 |
| 19 | [#28](https://github.com/caioplaninschek/presente/issues/28) | #7 | Gabriel | 21/09 |
| 20 | [#30](https://github.com/caioplaninschek/presente/issues/30) | #7 | Caio | 22/09 23:59 |

Epics no GitHub: [#5](https://github.com/caioplaninschek/presente/issues/5) Entrega 04 · [#6](https://github.com/caioplaninschek/presente/issues/6) 21/09 · [#7](https://github.com/caioplaninschek/presente/issues/7) 22/09 · [#8](https://github.com/caioplaninschek/presente/issues/8) 29/09 · [#9](https://github.com/caioplaninschek/presente/issues/9) 05/10.

**Onde os vértices estavam em 15/09.** O 06 fechou com o merge do PR #29. Os vértices 12, 13, 15 e 17 têm o **código preparado e compilando** — o Cauã preparou os quatro em 15/09 —, e o que falta nos quatro é rodar na placa, que depende do 08 (protoboard). O João avisou em 15/09 que o **RC522 precisa de solda nos pinos**: isso não trava o 12, que usa só LED e buzzer, mas trava o 13, o 15 e o 17, que são os testes 1, 2 e 8 do §8.

~~⚠️ **O 04 continua aberto**, vencido desde 14/09 e sem uma linha do João em issue nenhuma. Ele é a tarefa zero e trava **tudo** que sobe na placa, o 12 inclusive — a solda não é a única trava da quinta. Cobrado na #14 e no grupo em 16/09.~~ O 04 funcionou, e o João registrou na #14 em 21/09 (ver o bloco de 21/09 abaixo).

**A solda ficou para sábado (17/09).** O João avisou no grupo que só consegue soldar os pinos do RC522 no sábado, 19/09. O que não depende dela — a parte sem leitor do 08, o 11 e o 12 — passa a ter prazo até sábado, com o 12 rodando assim que o 04 funcionar. O 13 roda no mesmo sábado, depois da solda. O 15 e o 17 seguem no domingo, 20/09, e agora dependem de a solda sair no sábado.

**Onde os vértices estavam em 21/09, 14h.** A solda não saiu no sábado, e a tarde de domingo não aconteceu: tudo rodou na segunda. O **04** funcionou: a placa aparece como CH340 na COM3, sem driver a instalar, e o Upload só passa segurando o `BOOT`. O **12** rodou: as três cores bateram com o monitor serial, e o vídeo chegou pelo WhatsApp. O bip saiu fraco, e o **11** terminou aí: o buzzer entrou por um transistor TIP122, e a peça vai ser trocada (R44). O **14** passou, com 236,5 KB livres antes da conexão segura. O **16** teve uma rodada sem pulo de canal, que não vale, e precisa ser refeito; o **18**, o veredito, espera por ele. O **13**, o **15** e o **17** esperam a solda do RC522, que o João faz em 21/09. O **19** e o **20** fecham com o que chegar até terça.

## Onde mora o código de cada vértice (R28)

Cada programa de bancada tem a sua pasta em `src/` e o seu ambiente no `platformio.ini`, com o mesmo nome. O passo a passo do João diz qual ambiente escolher antes do Upload.

| Vértice | Issue | Pasta | Ambiente |
|---|---|---|---|
| 04 | #14 | `src/ambiente/` | `ambiente` |
| 05 | #15 | `src/radio/` | `radio` (rodado de novo pelos vértices 14 e 16) |
| 12 | #24 | `src/prova-de-vida/` | `prova-de-vida` |
| 13 | #25 | `src/leitor/` | `leitor` |
| 15 | #26 | `src/encostou/` | `encostou` |
| 17 | #27 | `src/persistencia/` | `persistencia` |

Código que dois programas dividirem sobe para `lib/<módulo do §5>/`.

## Não fragmentado ainda

Os epics #8 (estrutura física, 29/09) e #9 (dashboard e o aparelho falando com a nuvem, 05/10) ficam como issue única. A caixa depende do tamanho real da placa montada e o dashboard depende do banco com dados dentro; vértice escrito hoje precisaria ser reescrito. Graduam depois de 22/09, pela mesma regra que o `map.md` já usa para outubro e novembro.

## Riscos conhecidos, registrados na abertura

1. **Nove execuções físicas em onze dias, todas pelo João.** É o preço do modelo em que a placa não circula. Se empilhar, a saída é uma tarde na casa dele com mais gente, ou a placa circular por alguns dias.
2. **Sem aula presencial na terça 15/09.** Some o único encontro garantido da semana, e é dele que o teste 16 (7c) depende, por precisar de dois celulares juntos.
3. ~~**O Igor ainda não é colaborador do repositório** — convite pendente desde 06/09. A issue #20 segue sem assignee por causa disso (convite ainda pendente em 13/09)~~ — **fechado em 15/09**: o convite foi aceito, o Igor tem permissão de escrita, a #20 está atribuída a ele e o trabalho dela chegou pelo PR #29. O epic #9 continua com o Caio.
4. **O Gabriel não se manifestou em nenhuma issue até 11/09.** Os dois vértices dele (01 e 19) foram escolhidos de propósito sem placa e sem código, e nenhum dos dois trava terceiro.
