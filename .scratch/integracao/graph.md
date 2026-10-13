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
  #32 montagem ──> #58 furo e extensão do LED   (4.7, sobra da caixa; aula de 13/10)

epic #9
  #35 banco acordado                     (descartado em 30/09, R60; vencia em 27/09)
  #36 contrato ──> #37 portal no contrato ──> #42 rodar P1 e #44 rodar P2, na mesma ida à placa
                   (a #37 é o que tira o aparelho de mentira do portal; a entrega do dashboard, #52,
                    leva o portal no pacote em que estiver, R55 e R69)
  #36 contrato ──> #41 escrever P1 ──> #42 rodar P1
                   #41 escrever P1 ──> #43 escrever P2 ──> #44 rodar P2
                   #43 escrever P2 ──> #45 escrever P3     (R70, 12/10: antes era #44 ──> #45)
                   #45 escrever P3 ──> #46 rodar P3 ──> #47 escrever P4 ──> #48 rodar P4
                   #44 rodar P2 ──> #46 rodar P3           (R70: a ordem das idas à placa não muda)
                   #48 rodar P4 ──> #49 escrever P5 ──> #50 rodar P5 e rodada completa
  #39 compras e UIDs ──> #40 cadastro dos cartões ──> #46 rodar P3
  #51 limpeza dos dados de teste ──> #48 rodar P4
  #38 página de histórico ──> #52 entrega do dashboard   (se a página entrar; R69: o que entra se decide ao montar a entrega)
```

**A aresta que destrava:** a #36 (contrato). O portal (#37) e o primeiro pacote do firmware (#41) dependem dela e, com ela pronta, andam ao mesmo tempo. *(12/10: a #36 fechou em 30/09. Hoje, o que destrava as idas à placa é a ligação do portal (#37), de que dependem as rodadas dos pacotes 1 e 2 (#42, #44); o pacote 3 (#45) já pode andar, pela R70.)*

**A aresta que o pacote 1 mudou (01/10, publicada em 11/10):** o pacote 2 (#43) passa a depender do pacote 1 publicado na `main` (#41), e não mais da rodada na placa (#42). A rodada do pacote 1 (#42) e a do pacote 2 (#44) esperam a ligação do portal (#37), porque só com ela o celular testa o portal de verdade, e as duas rodam na mesma ida à placa, sem data ~~até o cronograma novo da epic (#9)~~ *(12/10: esse cronograma nunca foi escrito; hoje, até o enunciado do protótipo completo, epic #53)*. Se essa ida atrasar além da data que o cronograma der a ela, o pacote 3 (#45) passa a depender do pacote 2 publicado (#43), pelo mesmo raciocínio. *(12/10: o gatilho nunca disparou, porque a data do cronograma nunca foi escrita, e foi dispensado pela R70. A #45 depende da #43 publicada, e a rodada do pacote 3 (#46) passa a depender também da #44, para a ordem das idas à placa não mudar.)*

**A aresta que serializa:** a placa. Todo "rodar" exige quem estiver com ela (R23), e cada rodada custa cerca de um dia. Por isso a rodada oficial do pacote só vai para a placa depois da revisão por alguém que não é o autor (R51). *(12/10, R68: quem escreveu o pacote e está com a placa pode testá-lo de modo informal antes, declarando isso na issue do pacote; a rodada oficial continua esperando a revisão.)*

**A aresta que o enunciado de 29/09 mudou:** o desenho (#34) vem antes da montagem (#32), e não depois dela, porque o enunciado pede o caminho "como imaginamos → como construímos" (R56). A entrega saiu da #34 para a #56, que depende do teste com a caixa fechada (#33). O modelo no Tinkercad (#57), pedido pelo professor fora do enunciado, não precisa da placa e corre ao lado do desenho. E o buzzer novo (#39) passou a entrar antes da montagem (R57), com a compra até 02/10.

**A aresta escondida:** a #39 (compras) antes da #46. Sem os cartões novos cadastrados, ~~o teste 2 (7 bytes) e~~ o teste 3 (crachá de fora da turma) ~~não têm~~ não tem crachá para rodar *(10/10: o teste 2 saiu, porque os crachás do projeto são de 4 bytes, R65)*.

## Epics

A lista das tarefas, com quem pegou cada uma, está nas epics #8 (estrutura física) e #9 (dashboard e o aparelho falando com a nuvem).

Epics esqueleto, sem tarefas até o enunciado sair: #53 (protótipo completo e vídeo), #54 (Semana da Computação) e #55 (relatório final).

## Onde mora o código (R51)

O firmware integrado mora em `src/presente/`, com o ambiente `presente` no `platformio.ini`. As bibliotecas compartilhadas continuam em `lib/` (`rfid`, `feedback`, `storage` e, desde a R66, `config`) e evoluem no lugar. Os programas de bancada de `src/<nome>/` ficam como estão.

## Como se pega uma tarefa (R52)

Atribua a issue a você e comente "peguei" antes de começar. A linha *Requer* diz o que ela exige, e só as de dono natural nascem atribuídas. Tarefa com prazo e sem ninguém a dois dias do vencimento: o Caio chama no grupo.

## Riscos conhecidos, registrados na abertura

1. **Cada rodada na placa custa cerca de um dia**, porque a placa não circula (R23). Mitigação: a revisão por outra pessoa antes da rodada oficial na placa (R51). *(12/10, R68: o teste informal de quem escreveu o pacote e está com a placa pode vir antes da revisão; só a rodada oficial espera por ela.)*
2. ~~**Semana de provas em 28 e 29/09.** O contrato (#36) e o pacote 1 (#41) caem logo depois dela.~~ *(12/10: passou. A semana de provas acabou em 29/09, e o contrato (#36) e o pacote 1 (#41) já foram escritos.)*
3. ~~**As datas dos enunciados ainda não saíram**, fora a de 06/10. Os prazos daqui são alvos.~~ *(12/10: o grafo não tem prazos desde 30/09, porque eles vivem nas issues. O enunciado do dashboard não saiu, e o grupo segue a interpretação do cronograma de 05/09, com alvo na aula de 13/10 (R69). As datas das epics do protótipo completo (#53), da Semana da Computação (#54) e do relatório (#55) são provisórias.)*
4. **A evidência do teste do painel é fraca** (R45). Ele se refaz com log na #50.
5. **O Supabase pausa sem uso.** ~~A #35 resolve, e vence antes da pausa prevista.~~ *(30/09: a #35 foi descartada, R60)* Mitigação: quem encontrar o banco pausado avisa o Caio, que o reativa pelo celular.
6. **Tarefa livre que ninguém pega.** A regra dos dois dias (R52) é a mitigação.

## Riscos acrescentados em 29/09, com o enunciado da estrutura física

7. *(12/10: passou. A caixa foi apresentada em 06/10 e está com o Caio, o leitor RC522 está com o João, e quem fica com a placa se decide na aula de 13/10.)* **A placa fica com a caixa ~~em 03 e 04/10~~ de 03 a 05/10** *(30/09: o teste com a caixa fechada passou para a segunda, R59)*. *(06/10: a montagem saiu em 05/10, e o teste com a caixa fechada passou para a terça, 06/10, R63)* O pacote 1 (#42) tinha alvo em 03/10 e cede ~~o fim de semana~~ o fim de semana e a segunda. Em 29/09, ninguém tinha pegado o contrato (#36) nem o pacote 1 (#41), e os alvos da epic #9 se refazem quando o enunciado do dashboard sair. *(11/10: a caixa já foi apresentada; a rodada do pacote 1 espera a ligação do portal (#37), sem data até o cronograma novo da epic (#9).)*
8. *(12/10: passou. A entrega da estrutura física foi postada pelos cinco no Teams, informação do Caio em 10/10.)* **Os cinco precisam postar.** O enunciado não aceita envio atrasado, e um integrante que não postar fica sem a entrega. Mitigação: os arquivos ficam prontos ~~no domingo (R58)~~ na segunda à noite, com rascunho no domingo *(30/09: R59; sobra só a terça para postar, custo aceito)* *(06/10: ficam prontos na própria terça, depois do teste com a caixa fechada, R63)*, e a #56 tem uma caixa de marcar por integrante.

## Riscos acrescentados em 12/10

9. ~~**A ligação do portal (#37) e a página de histórico (#38) não têm andamento.** Sem a #37, o portal servido pelo aparelho ainda fala com o aparelho de mentira (`data/mentira.js`), e não com o aparelho de verdade. Isso pesa na entrega do dashboard (#52, R69), cujo alvo é a aula de 13/10.~~ *(13/10: a #37 saiu — o portal fala por `fetch` com o aparelho, no §6.1, e o `mentira.js` só carrega com `?mentira=1`. Com isso, as rodadas dos pacotes 1 e 2 na placa (#42, #44) deixam de esperar por ela. A página de histórico (#38) está escrita e espera o Caio habilitar o GitHub Pages e pôr os segredos.)*
10. **A ida à placa das rodadas dos pacotes 1 e 2 (#42, #44) segue sem data.** As rodadas seguintes (#46, #48, #50) ficam sem data atrás dela, porque a #46 depende também da #44 (R70).
11. **A limpeza dos dados de teste (#51) pode ter de esperar a página de histórico (#38),** se ela usar as aulas `teste-` (passo 1 da #51). Apagar as aulas antes tiraria da página o que ela precisa para ser testada.
