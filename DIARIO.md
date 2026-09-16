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

**O banco de dados na nuvem entrou no ar (15/09).** Ele faz o papel do sistema acadêmico da UVA, guardando os alunos de cada turma e as chamadas que o aparelho envia, e é dele que a tela de histórico vai ler. A chave de acesso do aparelho fica gravada numa caixa presa à parede de uma sala sem vigilância, e quem conseguir extraí-la terá exatamente as permissões dela. Por isso, essa chave não lê nem altera nenhuma tabela diretamente: só pede a lista de uma turma e entrega o relatório de uma chamada, que o banco grava uma única vez, mesmo que o professor aperte Enviar de novo. O Caio tentou apagar e alterar registros com essa chave, e o banco recusou as duas operações. O histórico só pode ser lido com login. Os dados de teste são fictícios, exceto o nome e a matrícula dos integrantes do grupo, e as chamadas de exemplo serão apagadas antes da primeira aula real.

**O programa que lê o crachá ficou pronto para ir à bancada (15/09).** O leitor é a peça do protótipo que mais costuma dar trabalho, porque conversa com o microcontrolador por um barramento sensível a fio solto e a alimentação errada. Por isso ele foi isolado num programa que não faz mais nada: encostar o crachá e ver o número dele aparecer no monitor do computador, sem luz, sem som e sem gravar nada. O número sai no mesmo formato em que será guardado no banco, de modo que a lista anotada na bancada sirva diretamente como dado de teste. Os dois tamanhos de crachá que a equipe tem são aceitos, e nenhum dos dois é gravado — o crachá é apenas lido, para que o aluno continue podendo usá-lo em outros lugares.

A leitura propriamente dita foi escrita separada do programa, porque o programa seguinte desta semana, o que acende a luz verde e toca o bip, se apoia na mesma leitura e não deve reescrevê-la. O programa também informa, assim que é ligado, se o leitor não respondeu, e em que ordem conferir a montagem. Sem esse aviso, o leitor mudo e o crachá não reconhecido produzem a mesma tela vazia, e quem está na bancada não tem como distinguir um do outro.

O programa compila, mas ainda não rodou na placa: depende da montagem da protoboard, que é do João, e a gravação está marcada para sábado, 19/09.

**O LED e o buzzer ganharam o programa que os põe à prova (15/09).** Antes de ler qualquer crachá, é preciso saber que as peças de aviso obedecem. O programa percorre as três cores do LED, um segundo cada uma, apaga e toca o bip, e escreve no monitor do computador o nome da cor que está acesa naquele instante — de modo que quem estiver na bancada compare o que lê com o que vê, e descubra na hora se dois fios estão trocados entre si.

O mecanismo do LED e do bip foi escrito separado do programa, pela mesma razão que a leitura do crachá já tinha sido: é ele que o programa seguinte, o que dá o sinal de presença registrada, vai usar. A duração do bip aqui já é a que a especificação atribui ao sinal de presença, embora neste programa ela sirva apenas para ouvir a peça.

Um detalhe ficou preparado de propósito. O LED pode ser de dois tipos, que se ligam de maneira oposta, e o programa foi escrito para o tipo que a especificação prevê. Se for o outro, nenhuma cor acende, e a correção é uma única linha, indicada por escrito no próprio arquivo — quem estiver com a placa na mão não precisa procurar, nem esperar por quem escreveu.

Este programa também ainda não rodou na placa: depende da montagem da protoboard. A gravação está marcada para quinta, 17/09.

**O leitor e o aviso foram ligados um ao outro (15/09).** Com o crachá lido de um lado e o LED obedecendo do outro, o programa que junta os dois é o que faz o aparelho parecer um aparelho: o aluno encosta, o LED pisca verde duas vezes, o buzzer apita, e ele sabe que pode sentar. Quem já está registrado e encosta de novo recebe uma piscada verde só, sem som, e nada é gravado — a regra decidida no domingo, de uma presença por chamada. O crachá esquecido em cima do leitor não repete sinal nenhum.

O programa mede e imprime quanto tempo passa entre o crachá encostar e o aviso começar, porque a especificação promete que isso fique abaixo de dois décimos de segundo, e acima disso forma fila na porta da sala. A medição que a tarefa pedia, porém, cobre só o trecho final desse caminho e sairia perto de zero em qualquer circunstância, o que não provaria nada. O programa passou a imprimir também o trecho que contém o tempo de verdade, que é a conversa com o leitor de crachás. Nem esse número é o total exato, porque o tempo em que o crachá espera encostado antes de o programa ir buscá-lo não é visível de dentro do aparelho; ele é um piso, e está declarado como tal.

Com isso, os quatro programas desta frente estão escritos e o trabalho comum entre eles — a leitura do crachá, o LED e o buzzer, e a lista de quem já foi registrado — está em um só lugar, e não copiado em cada um.

Nenhum dos quatro rodou na placa ainda, porque a protoboard não foi montada. São quatro programas empilhados sobre a mesma fiação, e convém que os dois primeiros sejam gravados antes do domingo, e não os quatro na mesma tarde.

**As presenças passaram a sobreviver a uma queda de energia (15/09).** O aparelho vai ficar preso na parede de uma sala, ligado na tomada, e uma queda no meio da aula não pode obrigar o professor a refazer a chamada. Cada crachá lido passa a ser gravado na memória permanente da placa assim que é lido, e ao religar o aparelho relê o que gravou: quem já estava registrado antes do tranco continua registrado depois dele, e não é registrado de novo.

**Uma pergunta de formato precisou ser respondida antes de escrever, e ela não estava decidida.** A especificação mostra os registros de uma chamada dentro de uma lista, no formato em que o relatório sai do aparelho. Mas uma lista, para ser legível por um programa, precisa ter o seu fechamento reescrito a cada registro novo — e é exatamente essa reescrita que uma queda de energia não pode interromper, sob pena de o arquivo inteiro deixar de abrir. O modo seguro de gravar é o oposto: acrescentar no fim e nunca voltar atrás.

A saída adotada separa as duas coisas. No disco, cada registro é uma linha independente, acrescentada ao fim do arquivo; uma queda de energia estraga no máximo a última linha, que é descartada na releitura. A lista que a especificação mostra é montada na hora de exibir, compartilhar ou enviar o relatório, e continua idêntica ao que já estava combinado — nada muda para quem vai receber esses dados. Isso não contraria a especificação: ela já chama cada registro de "uma linha do arquivo" e já descreve a gravação como acréscimo ao fim.

O campo de horário recebeu um nome próprio, e não o nome que o relatório final usa. O horário de verdade vem do navegador do professor, que ainda não existe; até lá o que se grava é o tempo desde que a placa ligou, e chamá-lo pelo nome do outro faria o documento prometer uma coisa e o arquivo entregar outra.

As duas escolhas estão registradas no fórum do projeto, com data, para virarem decisão formal na próxima revisão da especificação.

**A conferência das entregas da semana fechou seis decisões na especificação (15/09).** O Caio releu as telas do Igor e os quatro programas do Cauã contra o que estava combinado, e o que era proposta ou medida provisória virou regra — incluindo as duas escolhas de formato do bloco anterior, que esperavam esta revisão.

As duas propostas do Cauã foram aceitas. Cada presença gravada continua sendo uma linha independente no fim do arquivo, e a lista que o relatório mostra é montada na hora de exibir, compartilhar ou enviar, idêntica ao que estava combinado. O horário que vai para o arquivo é o tempo desde que a placa ligou, com nome próprio; o horário de verdade chega depois, vindo do navegador do professor no início da chamada. O Caio corrigiu o motivo alegado para a primeira escolha: o sistema de arquivos volta ao último estado bom quando falta energia, de modo que a reescrita não deixaria o arquivo ilegível. A decisão continua de pé pelos outros dois motivos, a leitura barata e o desgaste menor da memória.

A medida do tempo entre o encostar e o aviso também foi fechada. Ela conta do início da conversa com o leitor, e o número vale como piso, porque o tempo em que o crachá espera encostado antes de o programa ir buscá-lo não é visível de dentro do aparelho. A ressalva veio do próprio Cauã, autor do programa: a tarefa pedia o trecho final do caminho, que sairia perto de zero em qualquer circunstância e não provaria nada. Se o piso passar de um décimo de segundo, o compasso com que o programa verifica o leitor é medido antes de qualquer veredito.

Nas telas, três pontos foram fechados junto. As telas entraram por revisão separada: a escolha de pôr as três numa página só passou pelo grupo antes de entrar no repositório. A exceção fica registrada para decisão que o grupo ainda não viu, e o caminho comum continua sendo o envio direto. Nenhuma tela avança sem a confirmação do aparelho: se ele não responder, a falha aparece na tela e o botão libera para tentar de novo. O portal e os registros dividem a mesma área de memória, e entrou no projeto a declaração que fixa o sistema de arquivos, sem a qual a montagem errada apagaria o portal recém-gravado.

**O programa do teste de rádio ficou pronto para a bancada (16/09).** O Caio escreveu o programa que vai dizer se o aparelho consegue manter a própria rede Wi-Fi no ar enquanto se conecta ao hotspot, que é o modo simultâneo. Sem ler crachá nem acender LED, ele sobe a rede do aparelho com uma página de login de mentira, reserva na memória o espaço da lista de uma turma de 40 alunos e, quando recebe o pedido, conecta no hotspot e abre uma conexão segura com o servidor do banco de dados na nuvem. Imediatamente antes dessa conexão, imprime quanta memória está livre e em que faixa o número caiu: abaixo de 40 KB, o modo simultâneo reprova; entre 40 e 45 KB, o resultado é suspeito; acima de 45 KB, passa. Junto sai quantos celulares estavam conectados ao aparelho naquele instante, porque sem nenhum o número sai otimista.

Ao planejar o programa, o Caio encontrou um impasse na própria tarefa. Ela dizia que a conexão só se abriria depois do login no celular do professor, mas no teste de memória o único celular disponível é o hotspot, ninguém faz login, e a medição nunca aconteceria. Por isso a medição pode ser pedida de dois lugares: por um botão na página do celular, no teste do painel, ou por uma letra digitada no monitor do computador, no teste de memória. O login sozinho não mexe no rádio.

O teste do painel verifica se o celular do professor continua na página quando o aparelho muda de canal para acompanhar o hotspot, e nada garantia que essa mudança acontecesse. Com os dois já no mesmo canal, a página continuaria aberta sem ter passado por prova nenhuma. Ao ligar, o programa procura o hotspot e põe a rede do aparelho num canal distante do dele, o que obriga a ligar o hotspot antes da placa; se os canais coincidirem mesmo assim, ele avisa que aquela rodada não vale. A página do celular mostra, a cada segundo, se o login continua valendo e há quanto tempo o aparelho respondeu pela última vez, e o monitor registra quando o celular sai da rede e quantos segundos leva para voltar, que é o número que o teste compara com o limite de cinco segundos.

Cada pedido faz uma única tentativa de conexão segura. Uma tentativa que falha deixa cerca de 4 KB de memória perdidos, e o número seguinte sairia contaminado; depois de uma falha, o programa só volta a medir quando a placa for reiniciada.

O programa compila, assim como os outros cinco do projeto, mas ainda não rodou na placa. As frases que ele imprime ficaram fixas, porque o roteiro de bancada, que é a próxima tarefa, vai citar cada uma delas.
