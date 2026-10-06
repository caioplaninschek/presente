# Execução: do plano de 23/09 até a entrega final

Criado: 23/09/2026
Origem: o plano PRESENTE-12, feito a partir da sessão de planejamento de 23/09/2026, e a rodada 16 da `docs/spec.md` (R46 a R55).

> **30/09/2026: os tickets locais desta pasta foram apagados** (continuam no histórico do git). Eles repetiam as issues, e as duas cópias se desencontravam. Desde então, o enunciado, o dono, o prazo e o andamento de cada tarefa vivem só na issue do GitHub. Este arquivo fica como mapa: o grafo usa os números das issues, e a tabela de-para, que repetia dono e prazo, saiu junto com os tickets.

## Por que este diretório existe

O grafo de `.scratch/prototipo/` fechou em 22/09, com a demonstração aprovada. O que falta até o fim do projeto é outra fase, com outras epics (#8 e #9) e outra regra de trabalho (R52), e ganha diretório próprio. ~~O formato continua o mesmo: tickets finos aqui, e o enunciado completo de cada tarefa na issue do GitHub.~~ *(30/09)* Os tickets daqui foram apagados; ver a nota do topo.

## O grafo

```
epic #8
  #31 ideias ──> #34 desenho ──> #32 montagem ──> #33 caixa fechada ──> #56 arquivos e os cinco postando
  #31 ideias ──> #57 modelo no Tinkercad ─────────────────────────────> #56
  #39 buzzer (epic #9) ──> #32 montagem

epic #9
  #35 banco acordado                     (descartado em 30/09, R60; vencia em 27/09)
  #36 contrato ──> #37 portal no contrato
  #36 contrato ──> #41 escrever P1 ──> #42 rodar P1 ──> #43 escrever P2 ──> #44 rodar P2
               ──> #45 escrever P3 ──> #46 rodar P3 ──> #47 escrever P4 ──> #48 rodar P4
               ──> #49 escrever P5 ──> #50 rodar P5 e rodada completa
  #39 compras e UIDs ──> #40 cadastro dos cartões ──> #46 rodar P3
  #51 limpeza dos dados de teste ──> #48 rodar P4
  #38 página de histórico ──> #52 entrega do dashboard
```

**A aresta que destrava:** a #36 (contrato). O portal (#37) e o primeiro pacote do firmware (#41) dependem dela e, com ela pronta, andam ao mesmo tempo.

**A aresta que serializa:** a placa. Todo "rodar" exige quem estiver com ela (R23), e cada rodada custa cerca de um dia. Por isso o pacote só vai para a placa depois da revisão por alguém que não é o autor (R51).

**A aresta que o enunciado de 06/10 mudou:** o desenho (#34) vem antes da montagem (#32), e não depois dela, porque o enunciado pede o caminho "como imaginamos → como construímos" (R56). A entrega saiu da #34 para a #56, que depende do teste com a caixa fechada (#33). O modelo no Tinkercad (#57), pedido pelo professor fora do enunciado, não precisa da placa e corre ao lado do desenho. E o buzzer novo (#39) passou a entrar antes da montagem (R57), com a compra até 02/10.

**A aresta escondida:** a #39 (compras) antes da #46. Sem os cartões novos cadastrados, o teste 2 (7 bytes) e o teste 3 (crachá de fora da turma) não têm crachá para rodar.

## Epics

A lista das tarefas, com quem pegou cada uma, está nas epics #8 (estrutura física) e #9 (dashboard e o aparelho falando com a nuvem).

Epics esqueleto, sem tarefas até o enunciado sair: #53 (protótipo completo e vídeo), #54 (Semana da Computação) e #55 (relatório final).

## Onde mora o código (R51)

O firmware integrado mora em `src/presente/`, com o ambiente `presente` no `platformio.ini`. As bibliotecas compartilhadas continuam em `lib/` (`rfid`, `feedback`, `storage`) e evoluem no lugar. Os programas de bancada de `src/<nome>/` ficam como estão.

## Como se pega uma tarefa (R52)

Atribua a issue a você e comente "peguei" antes de começar. A linha *Requer* diz o que ela exige, e só as de dono natural nascem atribuídas. Tarefa com prazo e sem ninguém a dois dias do vencimento: o Caio chama no grupo.

## Riscos conhecidos, registrados na abertura

1. **Cada rodada na placa custa cerca de um dia**, porque a placa não circula (R23). Mitigação: a revisão por outra pessoa antes de ir para a placa (R51).
2. **Semana de provas em 28 e 29/09.** O contrato (#36) e o pacote 1 (#41) caem logo depois dela.
3. **As datas dos enunciados ainda não saíram**, fora a de 06/10. Os prazos daqui são alvos.
4. **A evidência do teste do painel é fraca** (R45). Ele se refaz com log na #50.
5. **O Supabase pausa sem uso.** ~~A #35 resolve, e vence antes da pausa prevista.~~ *(30/09: a #35 foi descartada, R60)* Mitigação: quem encontrar o banco pausado avisa o Caio, que o reativa pelo celular.
6. **Tarefa livre que ninguém pega.** A regra dos dois dias (R52) é a mitigação.

## Riscos acrescentados em 29/09, com o enunciado da estrutura física

7. **A placa fica com a caixa ~~em 03 e 04/10~~ de 03 a 05/10** *(30/09: o teste com a caixa fechada passou para a segunda, R59)*. *(06/10: a montagem saiu em 05/10, e o teste com a caixa fechada passou para a terça, 06/10, R63)* O pacote 1 (#42) tinha alvo em 03/10 e cede ~~o fim de semana~~ o fim de semana e a segunda. Em 29/09, ninguém tinha pegado o contrato (#36) nem o pacote 1 (#41), e os alvos da epic #9 se refazem quando o enunciado do dashboard sair.
8. **Os cinco precisam postar.** O enunciado não aceita envio atrasado, e um integrante que não postar fica sem a entrega. Mitigação: os arquivos ficam prontos ~~no domingo (R58)~~ na segunda à noite, com rascunho no domingo *(30/09: R59; sobra só a terça para postar, custo aceito)* *(06/10: ficam prontos na própria terça, depois do teste com a caixa fechada, R63)*, e a #56 tem uma caixa de marcar por integrante.
