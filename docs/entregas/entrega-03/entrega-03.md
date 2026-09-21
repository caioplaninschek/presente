# Entrega 03 — Projeto detalhado da solução

> **Transcrição do documento entregue**, `Entrega 03 - 20260906 - Documento Projeto Detalhado.pdf` (5 páginas: capa + 4 de conteúdo). Entregue em 07/09/2026.
>
> A montagem final saiu do repositório: o documento foi formatado em ABNT no Word pelo Caio e comprimido pelo João para caber no limite de páginas. O `.docx` é a fonte editável e vive na pasta da disciplina, fora do repo. Esta transcrição existe para que o repositório registre o que o professor recebeu, palavra por palavra.
>
> O rascunho que alimentou o Word está em `entrega-03-rascunho.html`; ele **não** é o documento entregue e tem texto mais longo em várias seções.

---

## Capa

> Logotipo da UVA centralizado no topo.

UNIVERSIDADE VEIGA DE ALMEIDA
Bacharelado em Ciência da Computação

GABRIEL ALBUQUERQUE VARELA SANTARELLO - 1240110815
CAUÃ MANUEL PROENÇA DE ANDRADE - 1240109764
IGOR ROCHA LOBATO - 1240114118
CAIO PARADA OLIVEIRA PLANINSCHEK – 1240205596
JOÃO VICTOR BERÇOT CHABUDET CABRAL – 1240108001

**PRESENTE!**
DISPOSITIVO EMBARCADO PARA CONTABILIZAÇÃO DE PRESENÇA EM SALA DE AULA

(Trabalho da disciplina de Sistemas Embarcados)
Entrega 03 — Projeto detalhado da solução

Professor: Thiago Alberto Ramos Gabriel

Rio de Janeiro RJ
2026

---

## 1 IDENTIFICAÇÃO DO PROJETO

Presente! é um terminal embarcado para contabilização de presença em sala de aula, desenvolvido na turma 4172CMPN6A_P1. O problema escolhido é o tempo de aula consumido pela chamada manual e os erros de marcação que ela produz. Nas Entregas 01 e 02 o projeto aparece sob o nome SmartPresence, substituído em 06/09 pelo nome atual.

## 2 PROBLEMA

O problema é do professor que dá aula para turma cheia. Na UVA Barra a chamada é feita no site da própria instituição, numa tabela com duas caixas por aula, em que a caixa marcada vale falta e a caixa em branco vale presença. O professor lê a lista nome por nome, pergunta à turma quem está presente e marca as caixas conforme as respostas.

Cada chamada consome tempo e pode acarretar em erros. Um deslize de linha na tabela marca falta no aluno errado. Quanto maior a turma, mais tempo e mais esforço a chamada custa.

## 3 SOLUÇÃO PROPOSTA

O protótipo é um terminal fixo ao lado da porta, ligado à tomada. Ele mesmo cria a rede Wi-Fi pela qual é operado: o professor conecta o celular nessa rede, faz login e abre a sessão de chamada pelo navegador. A partir daí o aluno encosta o crachá no leitor e recebe a confirmação na hora, por luz e som. Encerrada a aula, o professor envia o relatório da sessão para uma API na nuvem.

O aparelho fala com a internet em dois momentos, e só neles: no login, quando baixa a lista da turma, e no envio, quando sobe o relatório. Descartamos a alternativa de consultar a API a cada crachá lido, porque numa turma de 40 alunos seriam 40 idas à internet. Entre os dois momentos o aparelho confere cada leitura na lista que já está na memória flash interna, sem rede nenhuma.

Sem internet no login, a sessão abre mesmo assim, em modo degradado: o aparelho registra apenas o UID, o número de série gravado no chip da tag, e marca a sessão como offline. Nome e matrícula são acrescentados no envio, do lado da API.

Uma mudança em relação ao Canvas Entrega 02: retiramos o registro de saída por segundo toque. Ele duplicaria o estado guardado por aluno e acrescentaria um ponto de falha. O protótipo registra presença; a ausência é o que não foi registrado até o encerramento da sessão.

## 4 COMPONENTES

| Componente | Qtd | Função |
|---|---|---|
| Microcontrolador ESP32 DevKit V1 | 1 | Controla o sistema: lê o módulo NFC, grava os registros, serve o portal do professor e conversa com a API |
| Leitor RFID/NFC RC522 (13,56 MHz) | 1 | Sensor — lê o UID da tag por aproximação (SPI VSPI: SCK 18, MISO 19, MOSI 23, SS 5, RST 22) |
| Tags NFC 13,56 MHz (NTAG215 e cartões Mifare) | 3 a 5 | Crachás de teste; o firmware apenas lê o UID e nunca escreve na tag |
| LED RGB de catodo comum ou 2 LEDs (Verde/Vermelho) | 1 | Atuador 1 — sinaliza o resultado de cada leitura e o estado da rede (GPIO 25, 26 e 27) |
| Resistor de 220 Ω | 3 | Limita a corrente de cada canal de cor do LED |
| Buzzer ativo de 5 V | 1 | Atuador 2 — bip de 100 ms na confirmação da leitura (GPIO 33) |
| Protoboard de 830 pontos | 1 | Montagem e interligação dos módulos |
| Jumpers macho-macho e macho fêmea | 1 kit | Ligações de sinal e de alimentação |
| Fonte de 5 V 2 A com cabo micro-USB | 1 | Alimenta o aparelho, que fica permanentemente na tomada |

O projeto integrador pede no mínimo dois sensores. A redução para um sensor foi acordada com o professor: o leitor RC522 permanece como sensor único, como já constava na Entrega 02.

## 5 DIAGRAMA DE BLOCOS

> Imagem do diagrama de blocos. Fonte editável versionada em `diagrama-blocos.drawio`.

**Figura 1** – Diagrama de blocos. A API encena o papel do sistema acadêmico da UVA, ao qual não temos acesso: é dela que vem a lista da turma e é para ela que vai o relatório.

## 6 FUNCIONAMENTO ESPERADO

Ligado na tomada, o aparelho sobe como rede Wi-Fi própria e acende o LED. O professor conecta o celular nessa rede, e o portal cativo abre a página de login no navegador. Validada a senha, o aparelho responde “carregando turma” e aproveita esse intervalo para passar o rádio ao modo cliente, baixar a lista da turma da API e voltar ao modo hotspot, a troca deverá levar de 5 a 15 segundos. Se o hotspot não voltar, o aparelho deverá tentar de novo duas vezes, piscar rápido e reiniciar — a sessão é gravada em disco antes de cada troca de rádio, de modo que as presenças já registradas sobrevivem ao reinício. Com o painel aberto, o professor clica em Iniciar, e a sessão abre na hora, sem rede, porque a lista da turma já está na memória flash. O LED fica verde fixo, e a partir daí valem as três regras de leitura:

1. **Quando o leitor RC522 detectar uma tag cujo UID está na lista da turma**, o ESP32 deverá gravar o registro com UID, matrícula, nome e horário, fazendo com que o LED pisque verde duas vezes e o buzzer emita um bip de 100 ms. A meta é que o intervalo entre a aproximação da tag e o bip fique abaixo de 200 ms. Um toque registra a presença nos dois tempos da aula.
2. **Quando o leitor detectar o mesmo UID dentro de 5 segundos** da leitura anterior, o ESP32 deverá descartar a leitura, fazendo com que o LED acenda vermelho por 300 ms, sem som. É o debounce que evita registro duplicado quando o aluno demora a afastar o crachá.
3. **Quando o leitor detectar um UID que não está na lista da turma**, o ESP32 não deverá gravar nada, fazendo com que o LED acenda vermelho por 1 segundo e o buzzer emita um sinal de 400 ms. Essa verificação só existe com a lista carregada; em modo offline toda tag lida é aceita, e o verde passa a significar apenas que a leitura ocorreu.

Durante a sessão, o professor pode exibir o relatório parcial a qualquer momento e pode lançar a presença de um aluno pela matrícula, quando ele esquecer o crachá; o lançamento manual fica marcado como tal no relatório, com o autor e o motivo. No fim da aula, o professor envia o relatório: o aparelho troca de rádio outra vez, sobe a sessão e os eventos para a API e volta ao modo hotspot. Encerrada a sessão, o aparelho apaga a lista da turma e volta à tela de login, pronto para o próximo professor. No aparelho fica só a turma da aula corrente, nunca o cadastro de alunos da instituição.

## 7 DIVISÃO DAS RESPONSABILIDADES DA EQUIPE

| Papel | Responsável | Responde por |
|---|---|---|
| Hardware | Gabriel Santarello | Montagem na protoboard, pinagem do leitor e dos atuadores, e alimentação |
| Programação | Cauã Andrade | Firmware do ESP32: leitura do UID, gravação em disco, máquina de estados, LED e buzzer |
| Dashboard e interface | Igor Lobato | Portal do professor servido pelo aparelho: login, painel da sessão e relatório |
| Integração, documentação e apresentação | Caio Planinschek | Troca de rádio, API e lista da turma; consolidação dos documentos de entrega |
| Estrutura física | João Victor Cabral | Caixa de parede e fixação; diário de bordo, fotos e vídeo |

## 8 CRONOGRAMA DA EQUIPE ATÉ 21/09

As duas semanas até 21/09 têm dois marcos: a Entrega 04, de 14/09, e o protótipo eletrônico da Entrega 05, de 22/09.

| Integrante | Até 14/09 | Até 21/09 |
|---|---|---|
| Gabriel | Esquema de ligações fechado e compras concluídas (LED, resistores, buzzer, jumpers e fonte) | Protoboard montada e conferida, leitor respondendo na bancada, alimentação estável |
| Cauã | Lógica do firmware descrita: máquina de estados, leitura do UID e gravação em disco | Leitura, debounce, gravação e feedback funcionando na placa |
| Igor | Telas do portal desenhadas: login e painel do professor | Portal servido pelo aparelho, com login e a lista de presenças da sessão |
| Caio | Documento da Entrega 04 consolidado e tabelas criadas na API | Troca de rádio e as duas chamadas à API funcionando: lista da turma e envio do relatório |
| João | Material da caixa definido e diário de bordo em dia | Caixa cortada e aparelho fixado, com a antena do leitor acessível pela frente |

## Principal desafio técnico

O maior risco do projeto está no rádio: o ESP32 precisa alternar entre hotspot e cliente sem derrubar o portal do professor, e precisa manter o relatório íntegro quando essa troca falhar.

O ESP32 tem uma antena só. Manter o hotspot e a conexão externa ligados ao mesmo tempo faz o hotspot acompanhar o canal da rede externa e derrubar o celular do professor, e por isso os dois modos precisam se alternar. A pesquisa técnica que fizemos antes desta entrega mostrou que cada alternância leva de 5 a 15 segundos e há casos conhecidos, registrados no repositório do núcleo do Arduino para o ESP32, em que o hotspot não volta. A memória agrava o quadro: o handshake da conexão segura com a API exige de 40 a 50 KB de heap livre, o que torna obrigatório fechar o hotspot antes de abrir a conexão, por memória e não apenas por rádio.

Concentrar a rede em dois momentos é o que torna o problema tratável, porque em vez de uma janela de risco a cada crachá lido sobram duas na aula inteira. Em troca, o modo offline degradado e a rotina de recuperação passam a ser peças que precisam funcionar — quando a troca de rádio falha, o que está dentro do aparelho é uma aula inteira de presenças.

---

## O que a compressão do João retirou

Registrado porque o texto integral está no rascunho e pode ser reaproveitado nas próximas entregas — e porque parte do que saiu é informação que o relatório final de 30/11 vai precisar de volta.

- **§1** — a lista dos cinco integrantes. O enunciado a pede nesta seção; ela continua na capa.
- **§2** — os 10 a 15 minutos por chamada, a fraude por resposta de terceiro e o esforço vocal do professor. Sobrou só o erro de linha.
- **§3** — o botão Compartilhar como saída quando a internet falha; a fila na porta refém do Wi-Fi; e a frase que dizia que, em modo offline, o aparelho aceita qualquer tag por não saber quem é da turma.
- **§6** — as cores do LED nos estados de rede (azul fixo, azul lento, azul rápido) e o Compartilhar no fim da aula.
- **Desafio técnico** — a comparação entre os 5 a 15 segundos levantados na pesquisa e os 3 segundos estimados antes dela.

## Divergência declarada depois da entrega

Decisões posteriores a 07/09 deixaram três pontos deste documento desatualizados. O texto entregue fica como está; a spec registra cada mudança, e a Entrega 04 contou as duas primeiras ao professor.

- **§6 e desafio técnico — a alternância de rádio.** O documento trata a troca entre hotspot e cliente como o único arranjo possível. Em 10/09 a R19 mostrou que o modo simultâneo é nativo do ESP32: a alternância virou o Plano B, e a escolha sai dos testes de bancada. A Entrega 04 declarou a correção na seção 1.
- **§7 e §8 — a divisão por papéis e o cronograma por integrante.** Em 11/09 a R23 passou a distribuir o trabalho em tarefas com dono e prazo, porque há uma placa só e ela fica com o João. A seção 7 da Entrega 04 traz o checklist que substitui o §8, e a spec declara a divergência ao pé do §9.
- **§6, regra 2 — o mesmo crachá em 5 s.** O documento descarta a leitura repetida dentro de 5 s, com vermelho de 300 ms. Em 13/09 a R30 trocou isso por uma presença por sessão: quem já está registrado recebe verde de uma piscada, e nada é gravado, em qualquer momento da aula.
