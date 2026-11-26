# 2 O FIRMWARE

O firmware do Presente! é escrito em C++ sobre o núcleo do Arduino para o ESP32 e compilado pelo PlatformIO no VS Code. Fixamos as versões de tudo o que entra na compilação: a plataforma espressif32 7.1.3, que traz o núcleo do Arduino 2.0.17, e as bibliotecas MFRC522 1.4.12 e ArduinoJson 6.21.6. O programa principal mora em `src/presente/`, e o código que ele divide com os programas de teste que escrevemos antes, um para cada peça, mora em `lib/`. O Quadro 3 mostra a divisão do firmware em módulos, cada um com uma responsabilidade.

**Quadro 3: Os módulos do firmware**

| Arquivo | Responsabilidade |
|----|----|
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

O `loop()` nunca chama `delay()`: o LED, o buzzer e as esperas se medem pelo tempo decorrido em `millis()`, para que o aparelho continue atendendo o portal enquanto pisca. Cada evento da chamada vai para o arquivo `/eventos.json` como uma linha, acrescentada ao fim, e o arquivo nunca é reescrito; assim, uma linha cortada por queda de energia é descartada na leitura, com aviso. O horário de cada linha é o tempo desde que a placa ligou, porque o aparelho não tem relógio próprio, e a hora do relógio só se calcula na saída, a partir da hora que o navegador do professor manda.

## 2.1 A máquina de estados

O LED e o buzzer mostram em que estado o aparelho está, e é por eles que o aluno sabe se a presença foi registrada (Quadro 4).

**Quadro 4: Os estados do aparelho**

| Estado | LED | Som | Quando |
|----|----|----|----|
| `AGUARDANDO_LOGIN` | azul fixo | nenhum | ao ligar sem chamada guardada, e depois que o professor encerra a chamada |
| `SINCRONIZANDO` | azul lento | nenhum | o aparelho busca a lista da turma depois do login |
| `AGUARDANDO_INICIAR` | azul fixo | nenhum | a busca terminou, com ou sem a lista |
| `SESSAO_ABERTA` | verde fixo | nenhum | o professor clicou Iniciar |
| `REGISTRADO` | verde, duas piscadas | bip de 100 ms | o crachá foi lido e gravado |
| `JA_REGISTRADO` | verde, uma piscada | nenhum | o crachá já estava registrado nesta chamada, e nada é gravado |
| `NAO_RECONHECIDO` | vermelho por 1 s | buzz de 400 ms | o crachá não é da turma, e nada é gravado |
| `CHAMADA_ENVIADA` | azul fixo | nenhum | a chamada chegou ao banco, e o leitor deixa de registrar |

Fonte: elaborado pelos autores (2026).

Antes do Iniciar, o leitor não é consultado. Com a chamada aberta, no modo online, a lista da turma chegou do banco, e o crachá que não está nela recebe o vermelho; no modo offline, a lista não chegou, todo crachá lido é aceito, e o banco decide depois quem é da turma; o verde, nesse modo, quer dizer que o aparelho leu o crachá, e não que o aluno pertence à turma. Cada aluno tem uma presença só por chamada: o mesmo crachá lido de novo em até 5 s é ignorado sem sinal, e, passado esse tempo, um novo toque dá `JA_REGISTRADO`. Depois do Enviar, todo toque recebe o vermelho, nos dois modos.

## 2.2 Os cinco pacotes

Escrevemos o firmware em cinco pacotes, cada um numa issue do repositório (#41, #43, #45, #47 e #49) e rodado na placa em outra, porque o grupo tem uma placa só. Quem escreve um pacote confere o próprio código antes de publicá-lo, e a revisão por outro integrante ficou como condição para fechar a issue do pacote; decidimos assim em 14/10, porque a espera pela revisão não cabia no prazo de fechar o código até 20/10.

O primeiro pacote, o portal dentro do aparelho (#41), serve as páginas do portal e atende o login. Fora das rotas `/api/`, o aparelho só serve os arquivos .html, .js e .css que existem, porque a mesma partição guarda as contas dos professores, com o hash da senha, e os eventos da chamada, e servir qualquer arquivo os entregaria a quem os pedisse na rede do aparelho. Outra conta pode entrar com um professor já logado: antes do Iniciar, quem entra assume o painel, e, com a chamada iniciada, ela continua de quem a iniciou.

O segundo, a lista da turma pela internet (#43), busca a lista no banco logo depois do login, pelo hotspot do celular, com a rede do aparelho no ar o tempo todo. Cada busca faz uma conexão segura só, sem segunda tentativa, porque cada conexão segura que falha deixa presos cerca de 4 KB de memória. A lista vazia, a resposta com erro e a lista que não cabe nos 8192 bytes reservados para ela levam ao modo offline, e, no fim da busca, com ou sem a lista, o aparelho sai do hotspot.

O terceiro, a chamada (#45), registra a presença. O Iniciar só aceita a hora de Brasília num formato fixo, com o ano entre 2026 e 2099, e apaga os arquivos de uma chamada anterior antes de abrir a nova. Cada evento vai ao disco antes de o aluno entrar na memória de presentes e antes do sinal, para que o relatório, que se monta a partir do arquivo, nunca perca alguém que o painel contou; se a gravação falha, ninguém é registrado, e o toque recebe o vermelho. A gravação vem antes do sinal e o atrasa; o toque que passou dos 200 ms fica declarado nas limitações (Quadro 9). O monitor serial mostra o crachá ou a matrícula, mas nunca o nome do aluno, porque o registro do serial vai colado nas issues de um repositório público.

O quarto, o relatório (#47), monta o relatório lendo o arquivo de eventos uma linha por vez, envia-o ao banco e encerra a chamada. A chamada só fecha com a resposta do banco: com a confirmação de que gravou, ou de que a chamada já estava gravada, o aparelho passa a `CHAMADA_ENVIADA`; se o envio falha, a chamada continua aberta, e o professor pode tentar de novo ou salvar o relatório pelo Compartilhar. O Encerrar apaga a lista da turma e os arquivos da chamada e devolve o aparelho a `AGUARDANDO_LOGIN`.

O quinto, a queda de energia (#49), traz a chamada de volta: ao ligar, se o disco guarda uma chamada legível, o aparelho relê a lista da turma, se a chamada era online, e o arquivo de eventos, e volta à chamada, aberta ou enviada. Até o primeiro login com hora válida depois da queda, o LED fica em azul fixo, o leitor não é consultado, e o lançamento manual e o Enviar são recusados, porque um evento gravado antes desse login ficaria sem hora e, numa segunda queda, sairia do relatório ou faria o banco recusar o relatório inteiro.
