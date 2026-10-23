# Entregas — enunciados do professor

Fonte: Teams da disciplina, aba Tarefas. **O prazo de cada entrega é o do enunciado do professor.** As datas marcadas com \*\* ainda não têm enunciado publicado: vêm do cronograma passado ao grupo em 05/09 e se conferem quando o enunciado sair. Os enunciados das Entregas 03 e 04 citavam **21/09** como a data da preparação; o da Entrega 05 saiu em 20/09 e fixou **22/09, 23:59**. ~~O cronograma válido é o revisado (datas de terça); as datas de segunda são da outra turma~~ — informação errada, corrigida em 13/09.

| Data | Entrega | Estado |
|---|---|---|
| 27/08 | Formação da equipe, tema e problema | ✅ entregue |
| 02/09 | Canvas + lista de componentes + diagrama em blocos | ✅ entregue |
| 07/09 20:00 | Projeto detalhado da solução | ✅ entregue |
| 14/09 | Projeto de desenvolvimento: programação planejada, lógica, conexões, plano de testes | ✅ entregue |
| 22/09 23:59 | Integração sensor + atuador + testes documentados, com demonstração no laboratório | ✅ entregue e aprovada na demonstração |
| ~~29/09~~ 06/10 23:59 | Estrutura física do protótipo, com apresentação no laboratório ~~em data a confirmar~~ na aula de 06/10, informal *(30/09)* | ✅ entregue em 06/10, postada pelos cinco no Teams *(informação do Caio, 10/10)* |
| ~~05/10~~ ~~13/10~~\*\* | Dashboard (ESP32 obrigatório) ou interface Serial/Processing | ~~alvo do grupo, sem enunciado *(12/10)*~~ *(14/10, R71, R72)* sem entrega própria: vai na entrega final de 27/10; o documento da Entrega 07 não foi entregue |
| ~~20/10~~\*\* | ~~Protótipo completo + vídeo de demonstração (2 minutos)~~ | *(14/10, R71)* não há entrega em 20/10: passou para a entrega final de 27/10 |
| 27/10\*\*\* | **Entrega final:** protótipo completo, vídeo de demonstração (2 minutos), dashboard, exposição oficial na Semana da Computação e relatório técnico/documentação final | ⬜ *(16/10, R80)* sem enunciado escrito; o grupo adianta as cinco peças pela própria leitura, nas tarefas das epics #53, #54 e #55, e corrige no caminho o que um enunciado mudar (ver "Entrega final", abaixo) |
| ~~30/11~~\*\* | ~~Relatório técnico completo + versão final aprimorada~~ | *(14/10, R71)* não há entrega em 30/11: o relatório técnico vai na entrega final de 27/10 |

\*\* **Adiamento de 22/09.** Na aula, o professor adiou a estrutura física de 29/09 para **06/10**, porque 29/09 é dia de prova, e avisou que as entregas seguintes andam cerca de uma semana. A única data nova dita com certeza é 06/10, e o enunciado da estrutura física, recebido em 29/09, a confirmou. As outras não se somam de cabeça: uma semana depois de 05/10 cai no feriado de 12/10, e uma semana depois de 20/10 cai em 27/10, o dia da Semana da Computação, que é evento da universidade e pode não mudar. Cada data se confirma no enunciado. *(14/10: o calendário de 20/10 em diante foi refeito na aula de 13/10; ver a nota \*\*\* abaixo, R71.)*

*(12/10, R69)* **O enunciado do dashboard não saiu**, embora devesse ter saído junto com a aula de 06/10, pelo atraso de uma semana avisado em 22/09. Sem ele, o grupo entrega o dashboard pela própria interpretação do cronograma de 05/09 ("Dashboard (ESP32 obrigatório) ou interface Serial/Processing"), com alvo na aula de terça, **13/10**. O que entra se decide na tarefa da entrega do dashboard (#52). A diferença para as outras linhas marcadas com \*\*: ~~05/10~~ era a data do cronograma; **13/10 é um alvo do grupo, e não um prazo de enunciado**. Se o enunciado sair, vale o dele. *(14/10, R72: o alvo de 13/10 caiu. O documento da Entrega 07 não foi entregue e vira base da parte do dashboard na entrega final.)*

\*\*\* *(14/10, R71)* **A entrega final reúne tudo em 27/10.** Na aula de 13/10 o professor disse que não haverá entrega na semana de 20/10, e que a entrega final, em 27/10, reúne o protótipo completo, o vídeo de 2 minutos, o dashboard, a exposição na Semana da Computação e o relatório técnico/documentação final. Não há mais entrega separada em 20/10 nem em 30/11. É fala do professor em aula, trazida pelo Caio; não há enunciado escrito, e a data se confere quando ele sair. Pela leitura do grupo do que o professor vem dizendo, o dashboard é uma página HTML para acompanhar o que o aparelho está fazendo e ver como ele reage (R72); a tarefa da entrega do dashboard (#52) é do Igor. Meta do grupo, e não prazo do professor: todo o código pronto até 20/10, e a semana de 20 a 27/10 só para a parte física e a documentação (R73).

Requisitos obrigatórios do projeto integrador, válidos para todas as entregas: microcontrolador **ESP32**; mínimo de **2 sensores** (⚠️ **dispensado para este projeto** — o João acertou com o professor que o RC522 basta como sensor único; a exigência continua publicada no Teams, e a Entrega 03 declara a dispensa em uma linha no §4 para não ser lida como descumprimento — ver R18 na spec); mínimo de **1 atuador**; programação em **C/C++**; estrutura física; documentação completa; dashboard ou interface de monitoramento. Diário de bordo semanal obrigatório.

---

## Entrega 03 — Projeto detalhado da solução

**Prazo: 07/09/2026, 20:00. Formato: PDF de 2 a 4 páginas.**

O PDF deve conter, nesta ordem:

**1. Identificação do projeto** — nome do projeto, nome dos 5 integrantes, **turma**, problema escolhido.

**2. Problema** — qual problema real o projeto pretende resolver. Explicar brevemente: quem possui esse problema; por que ele é relevante; como ele é resolvido atualmente.

**3. Solução proposta** — como o protótipo com ESP32 pretende resolver o problema. Descrever o funcionamento esperado do sistema.

**4. Componentes** — tabela no formato `Componente | Qtd | Função`.

**5. Diagrama de blocos** — o fluxo `Sensores → ESP32 → Processamento → Atuador`, mais os outros elementos necessários.

**6. Funcionamento esperado** — passo a passo, no formato *"Quando o sensor X detectar determinada condição, o ESP32 deverá…, fazendo com que o atuador…"*.

**7. Divisão das responsabilidades da equipe** — quem responde por hardware, programação, estrutura física, dashboard/interface, documentação/apresentação.

**8. Cronograma da equipe** — o que cada integrante pretende desenvolver **até 21/09**.

**Pergunta obrigatória a ser respondida:** *Qual é o principal desafio técnico que a equipe acredita que encontrará durante a construção do protótipo?*

### Como foi entregue

Documento transcrito em [`entregas/entrega-03/entrega-03.md`](entregas/entrega-03/entrega-03.md); PDF em [`entrega-03.pdf`](entregas/entrega-03/entrega-03.pdf), e o rascunho com o texto integral ao lado, na mesma pasta. Cinco páginas — capa mais quatro de conteúdo.

| Seção | Fonte na spec | Como saiu |
|---|---|---|
| 1 | cabeçalho | parágrafo único; turma `4172CMPN6A_P1`; os 5 nomes ficaram só na capa |
| 2 | §1 | comprimido: sobrou o erro de linha, saíram os 10–15 min, a fraude e o esforço vocal |
| 3 | §1 + §3 + §6 | — |
| 4 | §4 | um sensor só, com a linha declarando a dispensa |
| 5 | §3 | imagem exportada de [`diagrama-blocos.drawio`](diagrama-blocos.drawio) |
| 6 | §5 (máquina de estados) + §6 | 3 regras de leitura, sem as cores do LED nos estados de rede |
| 7 | §9 | — |
| 8 | §9 | — |
| Pergunta obrigatória | §10 | — |

**Divergências declaradas no próprio documento**, para não serem lidas como incoerência entre entregas:

- **Nome.** SmartPresence (Entregas 01 e 02) → Presente!. Declarada no fim do §1.
- **Controle de saída.** O Canvas da Entrega 02 prometia *"1º toque registra Entrada, 2º toque registra Saída"*; a spec o retirou em R8. Declarada no fim do §3.
- **Um sensor só.** Declarada no §4. Não é divergência com o passado: a Entrega 02 já listava apenas o RC522 — o SW-420 nasceu e morreu entre as duas entregas, sem nunca sair em documento.

**Formato de páginas.** As Entregas 01 e 02 foram numeradas contando a capa (2 e 5 páginas). A Entrega 03 saiu com capa mais quatro de conteúdo, para um teto de 4, e o professor aceitou; a Entrega 04 saiu no mesmo formato.

---

## Entrega 04 — Especificação e preparação para o desenvolvimento

**Prazo: 14/09/2026, 23:59. Formato: PDF de 2 a 4 páginas. Vários envios permitidos.**

O PDF deve conter, nesta ordem:

**1. Evolução desde a entrega anterior** — o que foi alterado ou definido desde a entrega de 07/09. Pode incluir troca de componentes, alteração do funcionamento, mudança no diagrama, alteração da estratégia e problemas encontrados no planejamento.

**2. Arquitetura do sistema** — o diagrama de blocos atualizado.

**3. Ligações do ESP32** — tabela no formato `componente | GPIO | direção`.

**4. Lógica do programa** — fluxograma. Não é para entregar código.

**5. Pseudocódigo simples.**

**6. Plano de testes** — tabela.

**7. Preparação para 21/09** — checklist com o que precisa estar pronto: ESP32, sensores, atuador, ligações, código inicial, fluxograma, componentes testados individualmente e divisão das tarefas definida.

⚠️ O modelo do enunciado pressupõe **dois sensores** (`Sensor 1 / Sensor 2 → ESP32 → Atuador`) nas seções 2, 3 e 5. A dispensa acertada com o professor (R18) foi repetida em uma frase, debaixo da tabela da seção 3.

### Como foi entregue

Documento transcrito em [`entregas/entrega-04/entrega-04.md`](entregas/entrega-04/entrega-04.md); PDF em [`entrega-04.pdf`](entregas/entrega-04/entrega-04.pdf), e o rascunho com o texto integral ao lado, na mesma pasta. Cinco páginas — capa mais quatro de conteúdo —, montadas no Word sobre uma cópia do `.docx` da Entrega 03. Para caber, o corpo saiu com entrelinha 1,15, e só as seções 1 a 3 foram cortadas: o fluxograma ocupa uma página inteira e divide o documento em blocos que fecham em página inteira, de modo que só o bloco das seções 1 a 3 ganhava página com corte.

| Seção | Fonte na spec | Como saiu | Página |
|---|---|---|---|
| 1 | §2 (rodadas 7 e 8), §9, §10 e o diário | cortada para 259 palavras; o R19 e o desafio de memória ficaram inteiros | 2 |
| 2 | §3 | imagem de [`diagrama-blocos.png`](diagrama-blocos.png) a 10 cm, só com a legenda | 2 |
| 3 | §4.1 | tabela com as nove linhas que têm GPIO e uma frase com a alimentação, o buzzer e a dispensa do segundo sensor | 2 |
| 4 | fluxograma | [`fluxograma-logica.png`](fluxograma-logica.png) a 16 cm, página inteira | 3 |
| 5 | §5 | 22 linhas, com os passos do Plano B marcados como no fluxograma | 4 |
| 6 | §8 | os 11 testes, integrais | 4–5 |
| 7 | checklist da divisão em tarefas de 11/09 | integral | 5 |

O que saiu por falta de espaço está listado ao pé da transcrição.

---

## Entrega 05: Integração sensor + atuador + testes documentados

**Prazo: 22/09/2026, 23:59. Vários envios permitidos. Demonstração presencial obrigatória, no laboratório, na aula do mesmo dia.** O enunciado saiu em 20/09 e não fixa formato nem limite de páginas.

O critério principal, nas palavras do professor: **Sensor → Processamento → Atuador → Teste → Evidência**. A equipe precisa saber explicar e demonstrar o que o sistema recebe, como processa a informação e que ação executa como resultado.

O envio no Teams deve conter:

**1. Descrição da integração.** Qual sensor, qual atuador, qual microcontrolador ou plataforma, e como sensor, processamento e atuador interagem.

**2. Código-fonte atualizado.** O programa que roda no protótipo, organizado e comentado nos trechos principais, com as alterações feitas desde a Entrega 04 identificadas.

**3. Testes documentados.** Fotografia do protótipo montado, vídeo curto do funcionamento, os valores obtidos pelo sensor, a descrição de cada teste, o resultado esperado ao lado do obtido, e os problemas encontrados com as respectivas correções.

**4. Demonstração no laboratório.** O envio no Teams sozinho não valida o protótipo. Na aula, a equipe leva microcontrolador, sensor, atuador, protoboard, jumpers, alimentação e o que mais o projeto exigir, apresenta o protótipo funcionando e roda pelo menos um teste prático diante do professor, que confere se o que está na mesa bate com o que foi documentado.

A dispensa do segundo sensor (R18) continua valendo, e o documento a repete em uma frase, como nas Entregas 03 e 04.

### Como foi montada

O documento saiu do rascunho em [`entregas/entrega-05/entrega-05-rascunho.md`](entregas/entrega-05/entrega-05-rascunho.md), com as quatro seções na ordem do enunciado, e a seção 3 é o próprio dossiê de testes (issue #28), escrito direto no rascunho em 22/09. A montagem e o envio foram a issue #30. **Enviado em 22/09**, com 11 páginas (capa + 10). O post no Teams levou o PDF e todos os arquivos de `docs/assets/testes/`: os vídeos da prova de vida e do sinal de presença, a foto do protótipo montado, o quadro do vídeo e a foto do monitor serial. O post foi só os anexos, sem texto. O PDF está em [`entregas/entrega-05/entrega-05.pdf`](entregas/entrega-05/entrega-05.pdf) e a transcrição em [`entregas/entrega-05/entrega-05.md`](entregas/entrega-05/entrega-05.md).

Dos testes, o 1 e o 7d passaram; o 2 saiu em parte (a tag de 7 bytes foi lida, mas não registrada no programa do sinal de presença); o 8 saiu em parte (dois crachás em vez de trinta toques); e o 7c não teve rodada válida.

### A demonstração

Feita na aula de 22/09, com o programa do sinal de presença (issue #26) gravado na placa. **O professor aprovou o protótipo.** Ele perguntou se o buzzer era ativo ou passivo e sugeriu que a falta de som viesse da alimentação em 5 V; o João respondeu que a peça também não funcionou ligada à parte num Arduino, e que suspeita do disco cerâmico (R44 na spec).

Depois da demonstração, o grupo refez na sala o teste do painel do professor (issue #18), que no documento entregue aparece como não obtido, e ele passou em iPhone e Android: vale o Plano A (R45 na spec, issue #19). O PDF entregue não muda; o resultado fica registrado aqui e na spec.

---

## Entrega 06: Estrutura física do protótipo

**Prazo: 06/10/2026, 23:59. Vários envios permitidos. Todos os cinco integrantes enviam no Teams**, e o professor avisou que não há prorrogação. O enunciado chegou ao grupo em 29/09.

A entrega leva o projeto do circuito que funciona para uma estrutura física que o acomode, proteja e posicione, respondendo à pergunta do professor: *como esse sistema seria construído fisicamente para ser usado numa situação real?* O acabamento não precisa ser profissional. O material é livre (MDF, papelão, acrílico, impressão 3D, EVA, madeira, reciclável, peça feita pela equipe), e o professor avisa que não avalia qual é o mais caro ou sofisticado, e sim se a estrutura serve à função. O critério, nas palavras dele: **funcionalidade + criatividade + adequação ao problema + integração com o sistema eletrônico**.

A versão física deve conter, sempre que se aplicar: o microcontrolador, os sensores, os atuadores, a estrutura de suporte, o posicionamento adequado dos sensores, a localização dos atuadores, a alimentação e as conexões, o espaço para os componentes eletrônicos e a **identificação dos principais elementos**. No exemplo de controle de acesso do enunciado, o sensor e o atuador ficam posicionados de acordo com o funcionamento esperado, e é o caso mais próximo do Presente!.

O enunciado pede, nesta ordem:

**1. Planejamento da estrutura.** Pelo menos um desenho, esboço ou modelo digital, feito antes ou durante a construção, que mostre o caminho *como imaginamos → como construímos*. O enunciado aceita desenho à mão, sketch, Tinkercad, Fusion 360, AutoCAD, SketchUp, Canva, PowerPoint ou outro programa. Fora do enunciado, o professor pediu também um modelo no Tinkercad (informação do Caio, 29/09).

**2. Documentação obrigatória no Diário de Bordo.** Fotos do início da construção, da montagem e da estrutura pronta; os materiais usados; as alterações feitas; os problemas encontrados e as soluções adotadas; os integrantes envolvidos; os testes realizados. O professor avisa que não basta registrar o resultado final.

**3. Teste do protótipo.** Depois de montada a estrutura, verificar se ela atrapalha o funcionamento eletrônico: se os sensores leem, se o atuador funciona, se os componentes estão firmes, se os fios estão organizados, se o microcontrolador está protegido, se dá para chegar à alimentação e ao USB quando preciso, se a estrutura permite manutenção e ajustes, e se o protótipo cumpre a função para a qual foi projetado. Toda alteração que o teste exigir vai para o Diário de Bordo.

**4. Evidência do funcionamento.** Vídeo curto, sem exigência de qualidade profissional, na ordem *estrutura física → sensores → processamento → atuador → resultado*.

**5. Entrega no Teams.** Uma postagem até 06/10 com: foto do protótipo completo, fotos do processo de construção, o desenho, o vídeo, os materiais usados, a descrição das alterações, os problemas e as soluções, e a atualização do Diário de Bordo.

**6. Apresentação no laboratório.** A equipe leva o protótipo físico e deve estar pronta para apresentar a estrutura, identificar os principais componentes, explicar por que a estrutura foi construída daquela maneira, demonstrar o funcionamento, realizar pelo menos um teste e contar as dificuldades da construção. ⚠️ O enunciado não diz a data da apresentação; o grupo se prepara para a aula de 06/10~~, e a data se confirma com o professor~~. *(30/09: o Caio confirmou a aula de 06/10. A apresentação é informal, e o grupo mostra a caixa ao professor.)*

A dispensa do segundo sensor (R18) continua valendo. Como o grupo organizou esta entrega está nas rodadas 17 e 18 da spec (R56 a R59), com os ajustes de 30/09 à tarde na rodada 19, e nas tarefas da epic #8.

### Como foi entregue

**Entregue em 06/10.** Os cinco integrantes postaram a entrega no Teams até 23:59 (informação do Caio, 10/10). Documento transcrito em [`entregas/entrega-06/entrega-06.md`](entregas/entrega-06/entrega-06.md); PDF em [`entrega-06.pdf`](entregas/entrega-06/entrega-06.pdf). São 21 páginas: capa, resumo, sumário e 18 de conteúdo, com as referências. Esta entrega não teve rascunho no repositório, e o que mudou depois dela está declarado ao pé da transcrição. O vídeo da montagem de 05/10, o teste do cartão com a tampa fechada, está em [`assets/estrutura/entrega-06-video-2026-10-05.mp4`](assets/estrutura/entrega-06-video-2026-10-05.mp4).

Na aula do mesmo dia, o grupo fez o teste com a caixa fechada (teste 9, #33), com o ESP32 já preso, e ele passou: o cartão foi lido com a tampa fechada (informação do Caio, 10/10). Esse resultado não está no documento entregue, que registra o teste de 05/10, com o ESP32 ainda solto. O paradeiro do vídeo de 28 s citado no documento, e dos três vídeos originais, é desconhecido. O furo do LED na tampa e os fios que o levariam até lá não foram feitos, e ficaram para a aula de terça, 13/10, numa issue própria (#58), com o João (informação do Caio, 10/10).

---

## Entrega final: protótipo completo, vídeo, dashboard, exposição e relatório técnico

**Data: 27/10/2026, na Semana da Computação. Sem enunciado escrito** *(16/10)*. *(22/10, R81: em 27/10, a exposição às 18h30, sem vídeo e sem postagem; o enunciado e o modelo do relatório ainda vêm)*

### O que o professor disse

Na aula de 13/10, o professor disse que não haveria entrega na semana de 20/10 e que a entrega final, em 27/10, reúne o protótipo completo, o vídeo de demonstração de 2 minutos, o dashboard, a exposição na Semana da Computação e o relatório técnico com a documentação final (R71). Disse também que deixaria o grupo livre para focar nessa entrega, que o tempo da aula serve para o grupo andar com o projeto e que ainda vai pedir documentos, sem saber se escreverá um enunciado (informação do Caio, 16/10).

### Como o grupo lê cada peça

Sem enunciado, o grupo adianta as cinco peças pela própria leitura e corrige no caminho o que um enunciado mudar (R80).

| Peça | Leitura do grupo | Onde está |
|---|---|---|
| Protótipo completo | o firmware dos cinco pacotes na placa, com a rodada completa dos testes do §8 | rodadas na placa da epic #9: a bateria dos pacotes 1 a 4 (#42, #44, #46 e #48) e a rodada completa (#50) |
| Vídeo de 2 minutos | ~~na ordem do vídeo da Entrega 06 (estrutura física, sensor, processamento, atuador e resultado), em duas gravações: as cenas com a placa na aula de 20/10 e a abertura com a caixa pintada depois~~ *(22/10, R81: o professor adiou o vídeo; as tarefas param até ele pedir)* | epic #53: o roteiro do vídeo (#59), a gravação (#60), a pintura da caixa (#61) e a edição (#62) |
| Dashboard | o portal do professor no aparelho e a página de histórico, como capítulo do relatório, a partir da Entrega 07 | a tarefa da entrega do dashboard (#52) |
| Exposição | a demonstração ao vivo, ~~com o vídeo como plano B,~~ *(22/10, R81: sem vídeo)* às 18h30, no andar de baixo, e o hotspot, a energia, a sala e o banco acordado no dia | epic #54: o roteiro da exposição (#63) e o dia da exposição (#64) |
| Relatório técnico | um documento só, na ABNT, no modelo das Entregas 05 e 06, com uma seção por arquivo | epic #55: o esqueleto (#65), as seções (#66 a #70), a consolidação (#71), o PDF (#72) e os arquivos da entrega final, ~~com a postagem~~ (#73) *(22/10, R81: o PDF espera o modelo do professor, os arquivos esperam o enunciado, e não há postagem)* |

O calendário é do grupo, e não prazo do professor: as seções do relatório até 23/10, ~~o vídeo editado até 24/10, o PDF até 25/10, a postagem até 26/10~~ e o dia 27/10 só para a exposição (R80). *(22/10, R81: o vídeo foi adiado, o PDF espera o modelo do professor, e não há postagem)*

### O formato do relatório

O grupo segue a ABNT, como em todas as entregas. ~~⚠️ O professor não falou de modelo nem de número de páginas, e a pergunta vai para a aula de 20/10.~~ *(22/10, R81: o professor disse em 20/10 que o relatório terá um modelo, que ele vai passar; o número de páginas não foi respondido)*

### Perguntas para a aula de 20/10

1. Vai sair enunciado escrito da entrega final? Que documentos ele quer além do relatório (o código, o diário de bordo, slides)?
   *Resposta (20/10):* vai sair enunciado escrito, mais adiante. Os documentos além do relatório não foram respondidos.
2. O relatório tem modelo (o da UVA, a ABNT de relatório técnico) ou limite de páginas? Pode ser um documento só, com o dashboard como capítulo?
   *Resposta (20/10):* vai ter modelo, e o professor vai passá-lo. Pelo entendimento do Caio, o relatório é para a semana seguinte a 27/10, e não para o dia da exposição. O limite de páginas e o documento único não foram respondidos.
3. Onde e até que horas se posta em 27/10? Os cinco postam, como na estrutura física?
   *Resposta (20/10):* não tem postagem.
4. Os 2 minutos do vídeo são o máximo? Ele vai como arquivo ou como link?
   *Resposta (20/10):* o professor pediu para adiar o vídeo de explicação. A exposição é sem vídeo.
5. A exposição: onde, a que horas, quanto tempo por grupo, se é para banca ou para público passando, se há mesa e tomada, e se podemos prender a caixa na parede (R64)?
   *Resposta (20/10):* às 18h30, no andar de baixo. É no formato de feira de ciências: o grupo fica atrás de uma mesa e mostra o projeto a quem passar, então a caixa vai na mesa, e não na parede (informação do Caio, 23/10). O tempo e a tomada não foram respondidos.
6. Ele quer operar o aparelho com uma conta própria? Se quiser, a conta dele se gera antes (§8).
   *Resposta (20/10):* o professor vai chamar outros professores para o evento. Se eles não operarem o aparelho, tanto faz quem opera: ele ou outra pessoa. A conta própria não se gera (R81).

**Sugestão do professor (20/10):** um Google Forms curto e anônimo, de seis perguntas, para quem passar pela exposição responder sobre o projeto. Ele vai pôr um modelo no Teams. O João ficou com o formulário e quer levar a ele por um cartão NFC (tarefa 7.3, R81).
