# Diário de bordo — Presente!

Registro semanal da evolução do projeto. Atualizado antes de cada entrega de terça.
O histórico de commits deste repositório é a prova primária; este arquivo é a versão legível.

---

## Semana 1 — 27/08/2026 · Entrega 01

**Entregue:** PDF de identificação da equipe, tema, introdução e objetivo.

Formação dos 5 integrantes e escolha do tema. A proposta do professor era resolver um problema real do campus da Barra; a partir de um comentário dele em aula, o grupo escolheu atacar o tempo gasto com chamada manual.

---

## Semana 2 — 02/09/2026 · Entrega 02

**Entregue:** PDF com Canvas do projeto, lista de componentes e diagrama em blocos.

Primeira formulação técnica: ESP32 + RC522 + LED + buzzer, com portal cativo Wi-Fi para o professor. O projeto se chamava **SmartPresence** nesta fase.

Duas coisas desta entrega foram revistas depois, e a revisão está registrada na spec: o controle de entrada/saída por dois toques (retirado) e o RTC como segundo sensor (substituído).

---

## Semana 3 — 07/09/2026 · Entrega 03

**Foco:** fechar a especificação antes de escrever qualquer linha de código.

Cinco rodadas de decisão sobre o produto, todas registradas em [`docs/spec.md`](docs/spec.md):

- **Nome.** SmartPresence → **Presente!**
- **Arquitetura de dados.** A lista da turma sai do aparelho e passa a vir da API. O aparelho deixa de guardar cadastro de aluno — só a turma da sessão corrente, apagada no encerramento.
- **Sincronização.** Descartada a chamada de API por aluno (40 idas à internet durante a aula). Passou para **dois momentos**: uma no login e uma no envio. Entre elas, tudo local e abaixo de 200 ms.
- **Segundo sensor: entrou e saiu na mesma semana.** RTC DS3231 descartado — era redundante, porque o navegador do professor já entrega a hora. No lugar entrou o **SW-420**, para detectar violação do aparelho, que fica fixo e sem vigilância numa sala de aula. Na manhã do dia 06, porém, o João confirmou com o professor que este projeto está dispensado da exigência de dois sensores, e o SW-420 saiu junto: ele não participa da chamada, e mantê-lo custaria compra, calibração de limiar no lugar definitivo e um estado a mais no firmware. O projeto fica com um sensor, o **RC522**.
- **Ferramentas.** PlatformIO no lugar do Arduino IDE; portal servido do LittleFS; credenciais fora do git.

**Pesquisa técnica concluída:** validado que o ESP32 consegue falar com o Supabase por HTTPS (`setInsecure()`, sem certificado pinado — a Supabase troca de autoridade certificadora sem aviso). Descoberto que a alternância entre modo hotspot e modo cliente leva **5 a 15 segundos**, não os 3 estimados, e que é o ponto de falha mais provável do projeto. O tratamento entrou na spec: sincronizar durante o login, duas tentativas de recuperação e reinício automático, com a sessão persistida em disco para sobreviver ao reboot.

**Infraestrutura:** repositório criado e publicado; mapa de decisões do projeto em `.scratch/presente/`.

**Entrega 03 fechada.** O documento do projeto detalhado ficou pronto cobrindo as oito seções do enunciado e a pergunta obrigatória sobre o principal desafio técnico. Duas divergências em relação às entregas anteriores foram declaradas no próprio texto, em vez de ficarem por conta do leitor: a troca do nome do projeto e a retirada do registro de saída por segundo toque, que o Canvas da Entrega 02 prometia. O diagrama em blocos foi redesenhado — o da entrega anterior mostrava o aparelho falando com o celular do professor e com a nuvem ao mesmo tempo, coisa que o rádio do ESP32 não faz. Na manhã do dia 06 ele foi refeito no **draw.io**, com o arquivo editável versionado em `docs/diagrama-blocos.drawio`: o diagrama deixa de ser desenho à mão em SVG e passa a ter fonte que qualquer integrante abre e ajusta. O SW-420 saiu do desenho junto com o resto do projeto.

**Entregue em 07/09.** A montagem final saiu do repositório: o Caio assumiu a formatação ABNT e fechou o documento no Word, e o João o comprimiu para caber no limite de páginas. A compressão custou detalhe em quase toda seção — os minutos gastos por chamada, o comportamento do aparelho quando fica sem internet, as cores do LED nos estados de rede. O que saiu ficou anotado ao lado da transcrição, porque o relatório final de 30/11 vai precisar dele de volta. A turma foi identificada e o único campo que estava em branco fechou.

**Próximo:** Entrega 04 (14/09) — programação planejada, lógica, conexões e plano de testes.

---

## Semana 4 — 14/09/2026 · Entrega 04

**Foco:** programação planejada, lógica, conexões e plano de testes.

**Repositório reconciliado com o documento entregue (08/09).** Até aqui o repositório guardava o rascunho que alimentou o Word, não o documento que o professor recebeu — as duas coisas já divergiam em turma, capa e no texto do §1. O documento entregue passou a ser arquivado no repositório, com transcrição em texto ao lado do PDF, e o rascunho foi renomeado para não disputar o lugar de fonte. A especificação passou a registrar a turma e o nome do professor, que até então não estavam em lugar nenhum do repositório.

**Especificação preparada para a Entrega 04 (08/09).** A especificação recebeu quatro blocos que ela não tinha e que a entrega desta semana cobra: as histórias de usuário do sistema, escritas do ponto de vista de quem usa — professor, aluno, equipe e o que acontece quando algo falha; a tabela de ligações elétricas, pino a pino, com o alerta de que o buzzer não liga direto no microcontrolador e precisa de um transistor; o pseudocódigo da lógica completa, do momento em que o aparelho é ligado até o encerramento da chamada; e a política de teste do projeto, que declara que a bancada é o único ambiente de teste e por quê — o que pode dar errado aqui é físico e de rádio, e não aparece contra um simulador.

**Decisões pendentes preparadas para o grupo (08/09).** As quatro decisões que ainda faltam foram reescritas em linguagem direta, cada uma com uma sugestão e o motivo dela, para serem debatidas pela equipe e não decididas por uma pessoa só. Duas dependem apenas de levantamento de material e vão para conversa direta; as outras — o que o banco de dados guarda e quem pode alterá-lo, e se vale construir uma segunda tela mostrando o histórico das aulas — passam a ser discutidas em tópicos abertos no próprio repositório, de modo que o debate fique registrado com data e autor.

**Uma dúvida do João virou o primeiro pedaço de código.** Ele perguntou como o aparelho sabe que deve devolver o professor ao painel, e não à tela de login, depois de derrubar a própria rede para buscar a lista da turma na internet. A resposta é que quem guarda a prova do login é o navegador do professor, não o aparelho: a troca de rádio derruba a conexão, não a sessão. Isso impôs um requisito de ordem que passou à especificação — o aparelho só pode desligar a própria rede depois que a resposta do login chegou inteira ao celular. O comportamento seguinte, porém, muda de um modelo de celular para outro e não se decide na teoria: ficou marcado como o primeiro teste de bancada do projeto, e ele precisa apenas do microcontrolador, que a equipe já tem em mãos.

**O grupo respondeu nos tópicos, e uma das respostas derrubou uma decisão da especificação (10/09).** Os quatro integrantes se manifestaram nos tópicos abertos no repositório, e o debate produziu o que a conversa por mensagem não vinha produzindo: uma correção técnica com fonte.

O João contestou a afirmação de que o microcontrolador não consegue ser rede e cliente de internet ao mesmo tempo, e a contestação procede. A documentação do fabricante mostra que o modo simultâneo existe e é nativo; a restrição real é de canal, porque as duas interfaces dividem a mesma antena e passam a operar no canal da rede externa, com um aviso padrão de migração que os celulares modernos acompanham sem perder a conexão. A decisão que a especificação registrava desde 05/09 — alternar entre os dois modos — partia de premissa errada e foi reaberta.

Isso mudou o formato do principal desafio técnico do projeto. A pergunta deixou de ser como alternar entre os modos sem derrubar o professor e passou a ser qual dos dois arranjos o hardware sustenta, porque o impedimento que sobrou não é de rádio, e sim de memória: a conexão segura com o banco na nuvem exige entre 40 e 50 KB livres no instante em que é aberta, e esse espaço precisa existir com a rede do aparelho, o servidor de páginas e a lista da turma carregados ao mesmo tempo. Os dois arranjos entraram no plano de testes como planos A e B, com dois critérios medidos na bancada, e a comparação passou a ser o primeiro código do projeto. O Cauã chegou de forma independente à mesma ordem de teste e acrescentou uma objeção aceita: a credencial da sessão não deve viajar no endereço da página, porque fica exposta em histórico, em captura de tela e em registro de acesso.

Nos demais tópicos, o modelo de dados ganhou três entidades por proposta do Cauã — professor com cadastro próprio, turma como tabela ligada ao aluno por uma tabela intermediária, e aula com estado —, sendo que o estado da aula é também o mecanismo que impede o envio duplicado do relatório. O Igor assumiu a tela de histórico. E o levantamento de material fechou: o João comprou todas as peças, que estão com ele, de modo que nenhuma frente do projeto depende mais de compra. A caixa saiu do caminho crítico e será decidida depois da montagem, quando o tamanho real do conjunto for conhecido.

**O trabalho foi dividido em tarefas pequenas, e a divisão mudou de forma (11/09).** Com a fase de decisão encerrada, a equipe passou do desenho de papéis para o de tarefas. A divisão de responsabilidades entregue em 07/09 descrevia cinco frentes com um dono cada, o que o enunciado daquela entrega exigia; na prática ela não sobreviveu ao primeiro obstáculo físico do projeto, que é haver um único microcontrolador para cinco pessoas.

A solução adotada foi inverter o movimento: em vez de o equipamento circular entre os integrantes, ele fica com quem já tem as peças, e são as tarefas que chegam até ele prontas para executar — o programa escrito, o passo a passo do que fazer, o que deve aparecer na tela e o que fazer quando não aparecer. Quem executa não precisa resolver o problema, só rodá-lo e relatar o resultado. Isso desfaz a fila de espera pelo equipamento e mantém quatro frentes andando ao mesmo tempo.

O trabalho ficou registrado como cinco conjuntos, um por entrega do professor, com dezenove tarefas dentro deles, cada uma com responsável, prazo e o que precisa estar pronto antes. As tarefas que dependem do equipamento nascem partidas em duas: preparar, que qualquer integrante pode fazer no próprio computador, e executar, que é de quem tem as peças.

**O teste do rádio foi partido em dois, e a ordem deles importa.** A medição de memória precisa de um celular apenas, e reprova sozinha o arranjo simultâneo caso o espaço livre fique abaixo do necessário. A verificação de que o painel do professor sobrevive à troca de canal precisa de dois celulares na mesma sala, e é o único item do projeto que depende de os integrantes estarem presencialmente juntos — o que ficou mais caro nesta semana, porque não haverá aula na terça.


**A especificação foi conferida contra as tarefas, e estava atrasada em três pontos (11/09).** Fechada a divisão em tarefas, a especificação foi lida item a item contra o que ficou atribuído a cada integrante. A semana produziu decisões em ritmo maior que o registro delas, e o risco concreto é a equipe passar a trabalhar por um combinado que o documento não conhece.

O primeiro ponto é a própria divisão de trabalho, que o documento ainda descrevia como foi entregue em 07/09. A tabela original permanece, porque é ela que a entrega anterior apresentou, e a mudança passou a ser declarada logo abaixo — mesmo tratamento dado antes ao controle de saída e ao segundo sensor. O segundo é um critério de teste que a tarefa correspondente havia afrouxado por conta própria, ao aceitar resultado parcial quando só houver celulares de um mesmo sistema. A decisão é razoável e agora consta da especificação, em vez de existir apenas na tarefa. O terceiro é a observação do Cauã sobre a credencial da sessão, aceita no dia anterior; ela estava registrada neste diário e não na especificação, e o material de discussão do grupo ainda apresentava como alternativa de reserva exatamente o que ele havia desaconselhado.

Na mesma conferência, o plano de testes passou a registrar o que ainda não tem responsável. Quatro verificações dependem de o aparelho já conversar com a nuvem ou do resultado do teste de rádio, e nenhuma delas cabe antes de 22/09. Em vez de ficarem faltando sem explicação, estão listadas com a data em que entram.

**Preparar o computador para trabalhar no projeto deixou de ser conhecimento de uma pessoa só (11/09).** O Cauã perguntou, no fórum do projeto, se havia um arquivo que desse à ferramenta de IA dele o mesmo contexto que as do resto da equipe recebem. Esse arquivo existe desde a criação do repositório, e a maioria das ferramentas o lê sozinha, mas em nenhum lugar estava escrito o que fazer para chegar até ele — e uma das que a equipe usa só o encontra com um aviso explícito.

O repositório ganhou um guia de preparação de máquina, com o passo a passo até o ambiente pronto e um teste que confirma se a IA está mesmo lendo o contexto do projeto. O guia fixa ainda a rotina de atualizar a cópia local antes de cada sessão de trabalho: nesta fase o material muda quase todo dia, e quem trabalha sobre cópia velha refaz o trabalho.

**O repositório passou a compilar (12/09).** O projeto ganhou a base de compilação do firmware, com as versões da plataforma e das bibliotecas fixadas, para que o mesmo código gere o mesmo resultado nas cinco máquinas da equipe. O núcleo do Arduino ficou de propósito na versão 2, e não na 3, mais nova: a troca mudaria o consumo de memória que o teste de rádio vai medir, e a pesquisa em que a especificação se apoia teria de ser refeita. Cada programa de teste passou a ter pasta e configuração próprias, para que os programas preparados em paralelo por integrantes diferentes não se apaguem e para que um teste possa ser repetido mais tarde sem reescrever nada. O primeiro programa só pisca o LED da placa e escreve uma frase por segundo, e existe para provar o caminho do computador até a placa. Este diário tinha anunciado, em 08/09 e em 10/09, que o teste de rádio seria o primeiro código do projeto; o primeiro acabou sendo este. A gravação é tarefa do João e ainda não aconteceu.

**O fluxograma da lógica ficou pronto (12/09).** O fluxograma do programa foi feito pelo Gabriel a partir do pseudocódigo e revisado para caber numa página. Ele percorre a lógica inteira, do aparelho ligado ao encerramento da chamada, e marca os passos que só existem se o teste de rádio indicar a alternância entre os modos; se o modo simultâneo for aprovado, esses passos saem do programa.

**Entrega 04 entregue.** O documento cobre as sete seções pedidas: o que mudou desde a entrega anterior, o diagrama de blocos atualizado, as ligações de cada pino do microcontrolador, o fluxograma, o pseudocódigo, o plano de testes e o que precisa estar pronto até 21/09. Na montagem, duas afirmações da especificação apareceram desatualizadas e foram corrigidas antes de chegarem ao documento: o diagrama ainda dizia que o rádio só alterna entre os modos, o que a correção de 10/09 derrubou, e a tabela de ligações dava como certo um transistor para o buzzer, que ainda depende de uma checagem na bancada. Para caber no limite de páginas, só as três primeiras seções foram cortadas, porque o fluxograma ocupa uma página inteira e divide o documento em blocos que fecham em página inteira. Saíram detalhes do texto de evolução, o diagrama ficou menor e o espaçamento entre linhas foi reduzido. O que saiu ficou anotado ao lado da transcrição, porque o relatório final vai precisar dele.

**Próximo:** Entrega 05 (22/09) — protótipo eletrônico, com os componentes testados e os testes documentados.

---

## Semana 5 · Entrega 05

**Foco:** protótipo eletrônico, com os componentes testados e os testes documentados.

**Uma conferência feita depois da entrega mudou a leitura do crachá e o calendário da semana (13/09).** Com a Entrega 04 postada, o Caio conferiu o repositório inteiro contra as tarefas abertas e tomou três decisões.

A primeira muda o que o aluno vê. Até aqui, o aparelho ignorava o mesmo crachá só nos cinco segundos seguintes à leitura, e um toque depois disso gravava um novo registro. Passa a valer uma presença por chamada: quem já está registrado e encosta de novo recebe uma piscada verde, sem bip, e nada é gravado, mesmo que o aparelho tenha reiniciado no meio da aula. O vermelho deixa de sinalizar a repetição e fica só para o crachá que não é da turma, porque o aluno já registrado que visse vermelho entenderia que houve erro. O pseudocódigo e o fluxograma da Entrega 04 ainda mostram a regra antiga.

As outras duas tratam de testes de rádio que saiam incompletos. Se a medição de memória cair na faixa em que o número não é confiável, o teste seguinte, em que o celular do professor fica conectado ao aparelho como numa aula, repete a medição nessa condição, sem sessão de bancada a mais. Se o teste do painel só puder ser feito com celulares de um mesmo sistema e passar assim, o modo simultâneo é adotado de forma provisória, e o outro sistema precisa ser testado até a entrega do dashboard.

A especificação também passou a registrar duas lacunas novas, que ficam para a integração depois desta entrega: o estado do aparelho depois do login e depois do envio do relatório, que o pseudocódigo não define, e o comportamento do celular do professor na janela de login que o sistema abre sozinho.

**O trabalho da semana passou a terminar em 21/09.** O prazo de cada entrega é o que o professor publica no enunciado. O desta entrega ainda não saiu, e os dois anteriores falam em preparação para 21/09. O programa que dá o sinal de leitura e o que mantém os registros depois de um reinício rodam no domingo, 20/09, na mesma tarde do teste do painel, que já exige dois celulares junto da placa, e o dossiê de evidências fica pronto na segunda, 21/09.

**As telas que o professor usa ficaram prontas e já rodam (14/09).** A chamada tem três telas: a de entrar, a de comandos e a do relatório. Elas abrem em qualquer navegador de celular, com uma turma inventada dentro delas, e por isso foram feitas e conferidas sem depender do aparelho, que ainda não foi montado e cujas peças estão com um dos integrantes. Elas ficam guardadas na área de arquivos que o microcontrolador serve, separadas do programa dele: ajustar uma tela não obriga a regravar o programa, e é o que permite às duas frentes andarem ao mesmo tempo.

As telas já cobram as regras que a equipe decidiu, em vez de deixá-las todas para o programa do aparelho. Com a lista da turma carregada, quem tenta lançar à mão a presença de alguém que já foi registrado recebe a recusa na própria tela, com o nome de quem já está presente e o aviso de que nada foi lançado. O envio do relatório é aceito uma vez só, de modo que um segundo toque no botão não produz um segundo registro. Encerrar apaga a lista da turma e as presenças da aula antes de devolver o aparelho à tela de entrar, e a senha do professor não aparece em momento nenhum no endereço da página.

Entre entrar e a tela de comandos existe uma espera, e ela é proposital: é nesse instante que o aparelho busca a lista da turma na internet, o que pode levar até quinze segundos. A tela conta os segundos e diz o que está acontecendo, para não ser lida como travamento; se o aparelho não responder, ela oferece fazer a chamada assim mesmo, sem a lista, que é o comportamento já previsto para a aula sem internet. Quanto tempo esperar antes de desistir é um valor de partida, a ajustar quando as telas rodarem dentro do aparelho.

O relatório sai no formato combinado para o banco de dados, e o professor pode salvá-lo no próprio celular quando o envio falhar. As cores seguem as da universidade.

Falta ligar as telas ao aparelho, o que depende do protótipo eletrônico desta entrega.
