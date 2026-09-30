# Execução: do plano de 23/09 até a entrega final

Criado: 23/09/2026
Origem: o plano PRESENTE-12, feito a partir da sessão de planejamento de 23/09/2026, e a rodada 16 da `docs/spec.md` (R46 a R55).

## Por que este diretório existe

O grafo de `.scratch/prototipo/` fechou em 22/09, com a demonstração aprovada. O que falta até o fim do projeto é outra fase, com outras epics (#8 e #9) e outra regra de trabalho (R52), e ganha diretório próprio. O formato continua o mesmo: tickets finos aqui, e o enunciado completo de cada tarefa na issue do GitHub.

## O grafo

```
#8   01 ideias ──> 04 desenho ──> 02 montagem ──> 03 caixa fechada ──> 23 arquivos e os cinco postando
     01 ideias ──> 24 modelo no Tinkercad ───────────────────────────> 23

#9   05 banco acordado                     (solto; vence em 27/09)
     06 contrato ──> 07 portal no contrato
     06 contrato ──> 11 escrever P1 ──> 12 rodar P1 ──> 13 escrever P2 ──> 14 rodar P2
                 ──> 15 escrever P3 ──> 16 rodar P3 ──> 17 escrever P4 ──> 18 rodar P4
                 ──> 19 escrever P5 ──> 20 rodar P5 e rodada completa
     09 compras e UIDs ──> 10 cadastro dos cartões ──> 16 rodar P3
     21 limpeza dos dados de teste ──> 18 rodar P4
     08 página de histórico ──> 22 entrega do dashboard
```

**A aresta que destrava:** o 06 (contrato). O portal (07) e o primeiro pacote do firmware (11) dependem dele e, com ele pronto, andam ao mesmo tempo.

**A aresta que serializa:** a placa. Todo "rodar" exige quem estiver com ela (R23), e cada rodada custa cerca de um dia. Por isso o pacote só vai para a placa depois da revisão por alguém que não é o autor (R51).

**A aresta que o enunciado de 06/10 mudou:** o desenho (04) vem antes da montagem (02), e não depois dela, porque o enunciado pede o caminho "como imaginamos → como construímos" (R56). A entrega saiu do 04 para o 23, que depende do teste com a caixa fechada (03). O modelo no Tinkercad (24), pedido pelo professor fora do enunciado, não precisa da placa e corre ao lado do desenho. E o buzzer novo (09) passou a entrar antes da montagem (R57), com a compra até 02/10.

**A aresta escondida:** o 09 (compras) antes do 16. Sem os cartões novos cadastrados, o teste 2 (7 bytes) e o teste 3 (crachá de fora da turma) não têm crachá para rodar.

## De-para

| Local | GitHub | Epic | Dono natural ou livre | Requer | Prazo-alvo |
|---|---|---|---|---|---|
| 01 | #31 | #8 | livre (todos mandam ideia; o grupo escolhe, e o Caio decide se ninguém responder; a ideia base do Caio vale se ninguém propuser outra) | nada | 01/10 e 02/10 |
| 02 | #32 | #8 | livre | a placa | ~~03-04/10~~ 03/10 |
| 03 | #33 | #8 | livre | a placa | ~~04-05/10~~ até 04/10 |
| 04 | #34 | #8 | livre | nada | ~~06/10~~ 02/10 (só o desenho, R56) |
| 05 | #35 | #9 | Caio | Supabase, admin do repo | 27/09 |
| 06 | #36 | #9 | livre | nada | 01/10 |
| 07 | #37 | #9 | Igor | nada | 07/10 |
| 08 | #38 | #9 | Igor | login de leitura | ≈11/10 |
| 09 | #39 | #9 | João | a placa | o buzzer até 02/10 (R57); os cartões ≈08/10 |
| 10 | #40 | #9 | Caio | Supabase | depois do 09 |
| 11 | #41 | #9 | livre | nada | ≈02/10 |
| 12 | #42 | #9 | livre | a placa | ≈03/10 |
| 13 | #43 | #9 | livre | nada | ≈06/10 |
| 14 | #44 | #9 | livre | a placa | ≈07/10 |
| 15 | #45 | #9 | livre | nada | ≈09/10 |
| 16 | #46 | #9 | livre | a placa e os cartões | ≈10/10 |
| 17 | #47 | #9 | livre | nada | ≈13/10 |
| 18 | #48 | #9 | livre | a placa | ≈14/10 |
| 19 | #49 | #9 | livre | nada | ≈17/10 |
| 20 | #50 | #9 | livre | a placa e dois celulares | ≈18-20/10 |
| 21 | #51 | #9 | Caio | Supabase | ≈13/10 |
| 22 | #52 | #9 | livre | nada | data do enunciado |
| 23 | #56 | #8 | livre | nada | arquivos em 04/10; os cinco postam até 06/10 |
| 24 | #57 | #8 | livre | uma conta no Tinkercad | 03/10 |

Epics esqueleto, sem tarefas até o enunciado sair: #53 (protótipo completo e vídeo), #54 (Semana da Computação) e #55 (relatório final).

## Onde mora o código (R51)

O firmware integrado mora em `src/presente/`, com o ambiente `presente` no `platformio.ini`. As bibliotecas compartilhadas continuam em `lib/` (`rfid`, `feedback`, `storage`) e evoluem no lugar. Os programas de bancada de `src/<nome>/` ficam como estão.

## Como se pega uma tarefa (R52)

Atribua a issue a você e comente "peguei" antes de começar. A linha *Requer* diz o que ela exige, e só as de dono natural nascem atribuídas. Tarefa com prazo e sem ninguém a dois dias do vencimento: o Caio chama no grupo.

## Riscos conhecidos, registrados na abertura

1. **Cada rodada na placa custa cerca de um dia**, porque a placa não circula (R23). Mitigação: a revisão por outra pessoa antes de ir para a placa (R51).
2. **Semana de provas em 28 e 29/09.** O contrato (06) e o pacote 1 (11) caem logo depois dela.
3. **As datas dos enunciados ainda não saíram**, fora a de 06/10. Os prazos daqui são alvos.
4. **A evidência do teste do painel é fraca** (R45). Ele se refaz com log no 20.
5. **O Supabase pausa sem uso.** O 05 resolve, e vence antes da pausa prevista.
6. **Tarefa livre que ninguém pega.** A regra dos dois dias (R52) é a mitigação.

## Riscos acrescentados em 29/09, com o enunciado da estrutura física

7. **A placa fica com a caixa em 03 e 04/10.** O pacote 1 (12) tinha alvo em 03/10 e cede o fim de semana. Em 29/09, ninguém tinha pegado o contrato (06) nem o pacote 1 (11), e os alvos da epic #9 se refazem quando o enunciado do dashboard sair.
8. **Os cinco precisam postar.** O enunciado não aceita envio atrasado, e um integrante que não postar fica sem a entrega. Mitigação: os arquivos ficam prontos no domingo (R58), e o 23 tem uma caixa de marcar por integrante.
