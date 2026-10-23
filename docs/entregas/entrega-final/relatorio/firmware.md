> **Seção do relatório técnico: o firmware (#67).** Quem escreve: Cauã. Fontes: o §5 da `docs/spec.md` e as decisões R51, R66 a R68 e R74 a R79, com as anteriores que elas pressupõem (R29, R30, R36, R41). Extensão: em branco até o professor responder sobre o número de páginas (ver o `indice.md`); hoje o texto tem dois quadros. A numeração da seção e das subseções fica com a consolidação (#71). Ficam de fora, por serem de outras seções: as medições e as rodadas na placa (testes, #69), a análise das limitações e os caminhos descartados (#70), a montagem e a pinagem como ligação física (estrutura física, #68) e o portal visto pelo professor (capítulo do dashboard, #52). Este bloco sai na consolidação.

# O FIRMWARE

O firmware do Presente! é escrito em C++ sobre o núcleo do Arduino para o ESP32 e compilado pelo PlatformIO no VS Code. Fixamos as versões de tudo o que entra na compilação: a plataforma `espressif32 7.1.3`, que traz o núcleo do Arduino 2.0.17, e as bibliotecas `MFRC522 1.4.12` e `ArduinoJson 6.21.6`. Ficamos na versão 6 do ArduinoJson porque, na versão 7, o documento estático passou a ocupar memória dinâmica sem aviso, e as regras do firmware exigem documento estático. O programa principal mora em `src/presente/`, e o código que ele divide com os programas de teste que escrevemos antes, um para cada peça, mora em `lib/`.

Descrevemos aqui o comportamento que o código escrito determina. A verificação de cada parte na placa é assunto da seção de testes.

## Os módulos

O Quadro 1 mostra a divisão do firmware em módulos, cada um com uma responsabilidade.

**Quadro 1: Os módulos do firmware**

| Arquivo | Responsabilidade |
|---|---|
| `config.h` | Os pinos, a turma, a disciplina, o endereço do aparelho na própria rede e os tempos de cada etapa |
| `storage.cpp` | A memória da placa: as contas dos professores, a chamada em curso, a gravação dos eventos e a lista da turma |
| `rfid.cpp` | A leitura do crachá, com a janela de silêncio de 5 s |
| `clock.cpp` | A hora, recebida do navegador do professor e somada ao tempo desde que a placa ligou |
| `chamada.cpp` | Quem está presente, a decisão sobre cada toque e cada lançamento manual, os contadores e o último toque |
| `relatorio.cpp` | O relatório da chamada, montado a partir dos eventos gravados |
| `portal.cpp` | O login, o cookie de sessão, as rotas que o portal do professor chama e o estado do aparelho |
| `net.cpp` | O rádio: a rede do próprio aparelho sempre no ar, e a ida ao hotspot para buscar a lista da turma e enviar a chamada |
| `feedback.cpp` | O LED e o buzzer, temporizados pelo tempo decorrido desde que a placa ligou |

Fonte: elaborado pelos autores (2026).

Algumas regras valem para todos os módulos. O `loop()` nunca chama `delay()`: o LED, o buzzer e as esperas se medem pelo tempo decorrido em `millis()`, para que o aparelho continue atendendo o portal enquanto pisca. O leitor RC522 fala com a placa pelo barramento SPI por hardware, e os documentos JSON têm tamanho fixo, sem concatenação de texto.

A memória da placa tem uma partição só, dividida entre as páginas do portal e os arquivos da chamada. Por isso o firmware nunca formata a partição quando a montagem falha, porque formatar apagaria o portal junto; a falha aparece como erro. Cada evento da chamada vai para o arquivo `/eventos.json` como uma linha, acrescentada ao fim, e o arquivo nunca é reescrito. Assim, a releitura se faz uma linha por vez, com pouca memória, e uma linha cortada por queda de energia é descartada na leitura, com aviso. O horário de cada linha é o tempo em milissegundos desde que a placa ligou, porque o aparelho não tem relógio próprio; a hora do relógio só se calcula na saída, a partir da hora que o navegador do professor manda.

## A máquina de estados

O comportamento do aparelho segue a máquina de estados do Quadro 2. O LED e o buzzer mostram em que estado ele está, e é por eles que o aluno sabe se a presença foi registrada.

**Quadro 2: Os estados do aparelho**

| Estado | LED | Som | Quando |
|---|---|---|---|
| `AGUARDANDO_LOGIN` | azul fixo | nenhum | ao ligar sem chamada guardada, e depois que o professor encerra a chamada |
| `SINCRONIZANDO` | azul lento | nenhum | o aparelho busca a lista da turma depois do login |
| `AGUARDANDO_INICIAR` | azul fixo | nenhum | a busca terminou, com ou sem a lista |
| `SESSAO_ABERTA` | verde fixo | nenhum | o professor clicou Iniciar |
| `REGISTRADO` | verde, duas piscadas | bip de 100 ms | o crachá foi lido e gravado |
| `JA_REGISTRADO` | verde, uma piscada | nenhum | o crachá já estava registrado nesta chamada, e nada é gravado |
| `NAO_RECONHECIDO` | vermelho por 1 s | buzz de 400 ms | o crachá não é da turma, e nada é gravado |
| `CHAMADA_ENVIADA` | azul fixo | nenhum | a chamada chegou ao banco, e o leitor deixa de registrar |

Fonte: elaborado pelos autores (2026).

Antes do Iniciar o leitor não é consultado, e o crachá encostado nesses estados não dá sinal nenhum. Com a chamada aberta, o aparelho trabalha em um de dois modos. No modo online, a lista da turma chegou do banco, e o crachá que não está nela recebe o vermelho. No modo offline, a lista não chegou, todo crachá lido é aceito e o banco decide depois quem é da turma; o verde, nesse modo, quer dizer que o aparelho leu o crachá, e não que o aluno pertence à turma. Depois do Enviar, todo toque recebe o vermelho, nos dois modos.

Cada aluno tem uma presença só por chamada. O mesmo crachá lido de novo em até 5 s é ignorado sem sinal, para que o crachá parado sobre o leitor não repita o bip; passado esse tempo, um novo toque dá `JA_REGISTRADO`.

## Os cinco pacotes

Escrevemos o firmware em cinco pacotes, cada um numa issue, e cada um rodado na placa em outra issue. A divisão existe porque o grupo tem uma placa só, que fica com o João, e cada rodada na placa custa cerca de um dia. Quem escreve um pacote confere o próprio código antes de publicá-lo, pelo meio que escolher, e a revisão por outro integrante ficou como condição para fechar a issue do pacote, sem segurar a rodada na placa nem o pacote seguinte. Decidimos assim em 14/10, porque a espera pela revisão não cabia no prazo de fechar o código até 20/10.

### O portal dentro do aparelho (#41)

O primeiro pacote faz o aparelho servir as páginas do portal e atender o login. O arquivo de configuração, com os pinos, mora numa biblioteca própria, `lib/config/`, porque as bibliotecas do leitor e do LED precisam lê-lo e não enxergam as pastas do programa principal. Três compilações numa cópia descartável do repositório comprovaram isso: nas outras duas posições, a biblioteca não achou o arquivo ou achou um homônimo do próprio framework. Com o arquivo nesse lugar, a pinagem deixou de estar repetida em dois arquivos, e os programas de teste, compilados antes e depois, saíram com as mesmas instruções.

Fora das rotas `/api/`, o aparelho só serve os arquivos `.html`, `.js` e `.css` que existem, e todo outro pedido é redirecionado para a página de login. A restrição protege a partição, que guarda também as contas dos professores, com o hash da senha, e os eventos da chamada, com o crachá de cada aluno; servir qualquer arquivo entregaria esses dois a quem os pedisse na rede do aparelho. O `.html` que não existe vai para o login porque é o que o iPhone e o Firefox pedem ao entrar na rede, e um erro no lugar do login apareceria na janela do portal cativo.

Decidimos depois, em 15/10, que outra conta pode entrar com um professor já logado. Antes do Iniciar, quem entra assume o painel. Com a chamada já iniciada, ela continua de quem a iniciou, e cada lançamento manual grava quem estava logado ao lançar. O caso comum é o professor que sai da sala sem encerrar, e recusar o login seguinte prenderia o aparelho a ele. Se o professor seguinte, ainda assim, der a aula dentro da chamada do anterior, as presenças vão para a sessão do outro.

### A lista da turma pela internet (#43)

O segundo pacote busca a lista da turma no banco logo depois do login, pelo hotspot do celular, com a rede do aparelho no ar o tempo todo. Cada busca faz uma conexão segura só, sem segunda tentativa, porque cada conexão segura que falha deixa presos cerca de 4 KB de memória, e duas tentativas não caberiam com folga no limite de 20 s da busca. A busca que falha leva o aparelho ao modo offline, que é o caminho previsto para a aula sem internet.

A lista vazia conta como falha. Uma turma com o código errado, ou sem aluno ativo, volta do banco como uma lista vazia, sem erro, e, tratada como lista carregada, faria o aparelho recusar todo crachá da aula. Também levam ao modo offline a resposta com erro, a resposta que não é uma lista legível e a lista que não cabe nos 8192 bytes reservados para ela.

No fim da busca, com ou sem a lista, o aparelho sai do hotspot. Se ele ficasse conectado durante a aula e o hotspot sumisse, o ESP32 o procuraria de canal em canal, a rede do próprio aparelho mudaria de canal junto, e o celular do professor poderia cair. Os tempos de cada etapa (7 s de espera pelo hotspot principal, 5 s pelo de reserva, 4 s para a conexão segura e 4 s para a leitura da resposta) somam os 20 s e dispensam um relógio à parte. São valores de partida, a calibrar na placa.

### A chamada (#45)

O terceiro pacote registra a presença. A regra de quem está presente fica no `chamada.cpp`, que devolve um veredito para cada toque sem conhecer o LED nem o portal, e o `portal.cpp` escolhe o sinal e a resposta.

O Iniciar só aceita a hora de Brasília num formato fixo, com o ano entre 2026 e 2099, e recusa o resto, como a hora de um celular com o relógio zerado. A conta dos dias é feita no próprio código, porque uma função da biblioteca padrão depende do fuso do sistema, e a outra usa um tipo de hora que, neste compilador, tem 32 bits e acaba em 2038. O Iniciar começa do zero: apaga os arquivos de uma chamada anterior e grava os dados da chamada nova. Se um desses arquivos não se apaga, o Iniciar é recusado e a chamada não abre, porque abrir por cima misturaria as presenças da aula anterior com as da nova, e o envio as mandaria ao banco com a identificação da chamada nova.

Cada evento vai ao disco antes de o aluno entrar na memória de presentes. Assim, o relatório, que se monta a partir do arquivo, nunca perde alguém que o painel contou. Se a gravação falha, ninguém é registrado, e o toque recebe o vermelho, para o aluno procurar o professor. A gravação vem antes do sinal e o atrasa; em dois toques, o atraso passou do tempo que a especificação pede, e esse limite fica declarado nas limitações. Com a lista da turma, a presença se reconhece pela matrícula, e o aluno lançado à mão que depois encosta o crachá recebe `JA_REGISTRADO`. Sem a lista, o aparelho não liga a matrícula ao crachá: o lançamento manual sempre grava, e o banco junta os dois no envio. A memória guarda até 64 presenças por chamada, e, cheia, nada mais se grava.

O monitor serial mostra o crachá ou a matrícula, mas nunca o nome do aluno, porque o registro do serial vai colado nas issues de um repositório público.

### O relatório (#47)

O quarto pacote monta o relatório da chamada, envia-o ao banco e encerra a chamada. O relatório sai um item por vez, lendo o arquivo de eventos uma linha por vez, porque com 40 alunos no modo online ele já passa de 8 KB, e no modo offline não há teto para o número de eventos. A mesma função serve às duas saídas: o Compartilhar escreve direto no celular do professor, e o Enviar escreve num arquivo temporário, que a biblioteca de HTTP manda inteiro.

Durante o Enviar, o aparelho fica ocupado pela tentativa inteira, com o LED em azul lento, e usa a mesma conexão segura só e os mesmos tempos da busca da turma. A chamada só fecha com a resposta do banco: com a confirmação de que gravou, ou de que a chamada já estava gravada, o aparelho passa a `CHAMADA_ENVIADA` e marca no disco que a chamada foi enviada. Se o envio falha, a chamada continua aberta, e o professor pode tentar de novo ou salvar o relatório pelo Compartilhar. Tentar de novo é seguro, porque o banco responde que a chamada já estava gravada quando recebe a mesma chamada outra vez.

Declaramos três limites deste pacote sem código para eles. A identificação da chamada tem resolução de minuto, e, de dois Iniciar no mesmo minuto, o banco grava só a primeira chamada. Se o banco grava o envio e a resposta se perde, a chamada segue aberta, e os toques seguintes ficam fora do banco, embora continuem no relatório do Compartilhar. No pior caso, a soma dos tempos passa dos 20 s, porque algumas etapas da conexão segura têm prazo próprio.

O Encerrar vale em qualquer estado com login, inclusive durante a busca da turma, que ele interrompe. Ele apaga a lista da turma e os arquivos da chamada, invalida o cookie e devolve o aparelho a `AGUARDANDO_LOGIN`. Assim, a lista da turma, com o nome e a matrícula dos alunos, só fica no aparelho até o Encerrar.

### A queda de energia (#49)

O quinto pacote traz a chamada de volta depois de uma queda de energia. Ao ligar, se o disco guarda uma chamada legível, o aparelho devolve ao relógio a hora e o tempo desde o boot registrados no Iniciar, relê a lista da turma, se a chamada era online, e relê o arquivo de eventos para saber quem já está presente. A linha cortada pela queda é descartada, com aviso no serial. A chamada volta aberta, ou enviada, se já tinha sido, com o professor que a iniciou no painel.

Até o primeiro login com hora válida depois da queda, o LED fica em azul fixo, ainda que a chamada esteja aberta; o leitor não é consultado, e o lançamento manual e o Enviar são recusados. Escolhemos esse caminho porque um evento gravado antes desse login ficaria sem hora e, numa segunda queda, sairia do relatório ou faria o banco recusar o relatório inteiro. O login com hora válida mede quanto o relógio do boot novo está atrás do relógio do Iniciar, e daí em diante os eventos são gravados na contagem do Iniciar. Com isso, o formato do arquivo de eventos e o formato do relatório não mudaram.

Declaramos também três limites deste pacote sem código para eles. Se o Encerrar não consegue apagar os dados da chamada, o próximo reinício volta à chamada encerrada, e é preciso encerrá-la de novo. A chamada online cuja lista da turma não volta do disco segue no modo offline, e os eventos de depois da queda saem só com o crachá, sem a matrícula. Se a conta do professor que iniciou a chamada for removida do aparelho entre a queda e a volta, o relatório continua com a identificação dele, e quem entra assume o painel. Os três dependem de uma falha da memória da placa ou de um arquivo trocado no meio da aula.
