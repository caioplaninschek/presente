# Entrega 04 — Especificação e preparação para o desenvolvimento

> **Transcrição do documento entregue**, `Entrega 04 - 20260913 - Documento Especificação e Preparação.pdf` (5 páginas: capa + 4 de conteúdo), arquivado em `entrega-04.pdf`. Aprovado em 13/09/2026 para a postagem no Teams, com prazo em 14/09, 23:59.
>
> O documento foi montado no Word sobre uma cópia do `.docx` da Entrega 03, de onde vêm a capa, as margens e os estilos ABNT, e cortado para caber em capa mais quatro páginas. O `.docx` é a fonte editável e vive na pasta da disciplina, fora do repo. Esta transcrição existe para que o repositório registre o que o professor recebeu, palavra por palavra.
>
> O rascunho que alimentou o Word está em `entrega-04-rascunho.md`; ele **não** é o documento entregue e tem texto mais longo nas seções 1 e 3.

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
Entrega 04 — Especificação e preparação para o desenvolvimento

Professor: Thiago Alberto Ramos Gabriel

Rio de Janeiro RJ
2026

---

## 1 EVOLUÇÃO DESDE A ENTREGA ANTERIOR

O obstáculo mais caro do período é físico. Há um microcontrolador só para cinco pessoas, e a divisão de responsabilidades de 07/09 descrevia cinco frentes com um dono cada. Em vez de o equipamento circular, ele ficou com o João, e são as tarefas que chegam até a placa prontas para executar. A tabela de responsabilidades da Entrega 03 descreve os papéis, mas o que distribui o trabalho hoje são tarefas com dono e prazo.

A primeira rodada de decisões tomadas por escrito corrigiu um erro da nossa própria especificação. O João contestou a afirmação de que o microcontrolador não pode ser rede e cliente de internet ao mesmo tempo, e a documentação do fabricante confirma que o modo simultâneo é nativo. A decisão de 05/09, de alternar entre os dois modos, foi reaberta.

O principal desafio técnico deixou de ser como alternar entre os modos sem derrubar o professor e passou a ser qual dos dois arranjos o hardware sustenta, porque o impedimento que resta é de memória: a conexão segura com a nuvem exige de 40 a 50 KB livres no instante em que é aberta, e esse espaço precisa sobrar com a rede do aparelho, o servidor de páginas e a lista da turma de pé. Os dois entraram no plano de testes com critérios de aprovação.

No mesmo período o modelo de dados ganhou três entidades propostas pelo Cauã, o João comprou todas as peças e a especificação recebeu as histórias de usuário, as ligações pino a pino, o pseudocódigo e a política de teste.

## 2 ARQUITETURA DO SISTEMA

> Imagem do diagrama de blocos, a 10 cm de largura. Fonte editável versionada em `diagrama-blocos.drawio`; imagem em `diagrama-blocos.png`.

**Figura 1** – Diagrama de blocos. O rádio fala com o celular do professor e com o hotspot ao mesmo tempo ou alternando entre os dois; a escolha sai dos testes de bancada. A API encena o sistema acadêmico da UVA.

## 3 LIGAÇÕES DO ESP32

| Componente | GPIO | Direção |
|---|---|---|
| RC522 (sensor) — SS (SDA) | 5 | Saída (SPI CS) |
| RC522 — SCK | 18 | Saída |
| RC522 — MOSI | 23 | Saída |
| RC522 — MISO | 19 | Entrada |
| RC522 — RST | 22 | Saída |
| LED RGB (atuador 1) — R | 25 | Saída |
| LED RGB — G | 26 | Saída |
| LED RGB — B | 27 | Saída |
| Buzzer ativo (atuador 2) | 33 | Saída |

O leitor trabalha estritamente em 3,3 V, fornecidos pelo regulador da placa; uma checagem de bancada vai dizer se o buzzer dispensa componente extra ou precisa de um transistor; e a tabela traz um sensor só porque o professor dispensou este projeto da exigência de dois.

## 4 LÓGICA DO PROGRAMA

> Imagem do fluxograma, a 16 cm de largura, com a nota do Plano B dentro da figura. Fonte editável versionada em `fluxograma-logica.drawio`; imagem em `fluxograma-logica.png`.

**Figura 2** – Fluxograma da lógica do programa.

## 5 PSEUDOCÓDIGO

```
INÍCIO: inicializar LittleFS, SPI, RC522, LED e buzzer
SE existe sessao_atual.json e a janela ainda está aberta: retomar a sessão
SENÃO: estado = AGUARDANDO_LOGIN
subir SoftAP (WPA2), DNS do portal cativo e WebServer
ENQUANTO ligado, sem delay: atender o portal, atualizar LED e buzzer
  SE o professor acabou de fazer login:
    responder "carregando turma...", estado = SINCRONIZANDO
    fechar o AP e ir para STA *
    SE baixou o roster: gravar em cache, modo = online; SENÃO: modo = offline
    voltar o rádio para AP *
    SE o AP não voltou em até 2 tentativas: estado = FALHA_DE_RADIO, reiniciar *
  SE clicou Iniciar: gravar sessao_atual.json, estado = SESSAO_ABERTA
  SE leu uma tag e a sessão está aberta:
    SE mesmo UID há menos de 5 s: estado = DUPLICADO, nada gravado
    SENÃO SE online e UID fora do roster: estado = NAO_RECONHECIDO, nada gravado
    SENÃO: gravar evento (origem nfc), estado = REGISTRADO
  SE lançou presença manual: gravar evento manual, com quem lançou e o motivo
  SE clicou Enviar: estado = SINCRONIZANDO, fechar o AP *
    enviar sessão e eventos para a API, com tentativas limitadas
    SE o envio falhou: manter eventos.json e deixar o Compartilhar disponível
    voltar o rádio para AP *
  SE clicou Encerrar: fechar a sessão, apagar o roster, estado = AGUARDANDO_LOGIN
```

Os passos marcados com * são a alternância de rádio do Plano B e deixam de existir se os testes de bancada aprovarem o Plano A.

## 6 PLANO DE TESTES

Os testes são feitos na bancada, com o aparelho real, e verificam só o comportamento externo: o que o LED e o buzzer fazem, o que o portal mostra e o que sai no relatório exportado e no log serial. Cada teste deixa evidência gravada, em foto, vídeo, arquivo exportado ou trecho do log.

| # | Teste | Critério |
|---|---|---|
| 1 | Crachá de 4 bytes (Mifare), três aproximações seguidas | Um registro só, com resposta em menos de 200 ms |
| 2 | Crachá de 7 bytes (NTAG215) | Registra pelo UID, sem escrever na tag |
| 3 | Modo online, crachá fora da turma | Vermelho longo e nada gravado |
| 4 | Modo offline, com o Iniciar sem internet | Sessão marcada como offline, todo UID aceito e relatório íntegro |
| 5 | Enviar sem internet e, depois, com internet | O Compartilhar funciona, e a nova tentativa sobe o mesmo relatório sem duplicar |
| 6 | Dois professores em sequência | Duas sessões, cada uma com o seu professor, sem mistura |
| 7 | Plano B — alternância de rádio, 10 ciclos seguidos | A rede do aparelho volta nas 10 vezes, o celular reconecta e cada troca leva de 5 a 15 s |
| 7b | Plano B — falha forçada na volta à rede do aparelho, com o hotspot desligado no meio | O aparelho se recupera sozinho e não perde as presenças já registradas |
| 7c | Plano A — modo simultâneo, com o professor no portal e o aparelho conectando ao hotspot em outro canal | O painel continua aberto sem novo login, em iPhone e em Android, e, se cair, reconecta sozinho em menos de 5 s; com dois celulares do mesmo sistema, o veredito sai parcial |
| 7d | Plano A — memória livre no instante da conexão segura, com a rede do aparelho, o portal e a lista da turma carregados | Pelo menos 40 KB livres, e abaixo disso vale o Plano B; medido sem celular conectado ao aparelho, o número sai otimista, e de 40 a 45 KB é suspeito |
| 8 | Estresse: 30 toques, com um reinício no meio | Nenhuma duplicata e nenhuma perda de registro |

Os testes 7 e 7b só passam a valer se a bancada indicar o Plano B. A retomada da sessão depois de um reinício e a máquina de estados completa ainda não têm teste e entram no plano depois de 22/09.

## 7 PREPARAÇÃO PARA 21/09

O ESP32, o leitor de crachá, o LED e o buzzer estão comprados e em mãos; as ligações estão na seção 3 e o fluxograma da lógica, na seção 4. O que falta — código inicial, componentes testados individualmente e divisão das tarefas definida — está na tabela abaixo. Esta entrega e o protótipo eletrônico da Entrega 05 são os dois marcos do período, e por isso algumas datas chegam a 22/09.

| Integrante | O que entrega até 21-22/09 |
|---|---|
| Gabriel | O fluxograma da lógica (13/09) e o dossiê de evidências dos testes (22/09) |
| Cauã | Os programas do leitor de crachá (19/09), do feedback de verde e bip, sem registro duplicado (21/09), e da gravação que sobrevive ao reinício (22/09) |
| Igor | As três telas do professor no navegador: login, painel de seis botões e relatório (21/09) |
| Caio | O ambiente de compilação no computador do João (14/09); o banco na nuvem, o programa e o roteiro de bancada do teste de rádio (17/09); e a escolha entre os dois arranjos de rádio (21/09) |
| João | A montagem da protoboard e a checagem do buzzer (16/09), a prova de vida do LED e do bip (17/09), a medição da memória livre na conexão segura (19/09) e a verificação do painel na troca de canal (20/09); e roda na placa, de 14 a 22/09, o que os outros preparam, gravando a evidência de cada teste |

Duas coisas travam o resto. Nada roda na placa antes de o ambiente de compilação estar de pé, em 14/09. E a verificação do painel é o único teste que exige dois celulares junto da placa, o que depende de um encontro combinado.

Os testes 3, 4, 5 e 6 do plano de testes não ficam prontos em 22/09: dependem de a lista da turma estar dentro do aparelho, o que só acontece quando ele passar a falar com a nuvem, entre o fim de setembro e o começo de outubro.

---

## O que ficou fora por falta de espaço

Registrado porque o texto integral está no rascunho e na spec, e parte do que saiu é informação que o relatório final de 30/11 vai precisar de volta. Para caber em capa mais quatro, o documento saiu com entrelinha 1,15 em vez de 1,5 e com o diagrama de blocos a 10 cm de largura.

- **§1** — a observação de que várias das cinco frentes precisavam da placa ao mesmo tempo; o programa escrito e o passo a passo que acompanham cada tarefa; os tópicos abertos no repositório como lugar das decisões; a premissa errada da decisão de 05/09; a frase que anunciava a mudança do desafio técnico; o que são as três entidades do Cauã (professor com cadastro próprio, turma como tabela e aula com estado, que impede o envio duplicado do relatório); e o Igor assumindo a tela de histórico.
- **§3** — a fonte de 5 V na tomada, que alimenta o conjunto; e as observações pino a pino da spec §4.1: o GPIO 5 como pino de *strapping*, os resistores de 220 Ω do LED e a ordem de checagem do buzzer antes de qualquer transistor.
- **§5** — duas ações do fluxograma que não couberam nas 22 linhas: abrir o painel com a lista em cache depois do login e abrir a sessão no Iniciar, que o estado `SESSAO_ABERTA` deixa implícita.
- **§6** — a política de teste da spec §8: por que não há teste automatizado, o log serial como instrumento de medição, quem roda cada teste e a calibração dos tempos como parte do teste.

## Divergência declarada depois da entrega (13/09)

O fluxograma e o pseudocódigo da spec ganharam duas linhas que o documento entregue não tem, ambas alinhamento ao R15 e ao R16, sem decisão nova: a volta do rádio depois do Enviar passa pela mesma checagem de duas tentativas do login, e a sessão retomada no boot fica em `SESSAO_ABERTA`. O PDF arquivado em `entrega-04.pdf` mostra o fluxograma como foi entregue; `fluxograma-logica.png` já traz a versão corrigida.

Na mesma data, a spec ganhou a rodada 10, e a **R30** mudou a leitura do crachá: quem já está registrado na sessão não se registra de novo em momento nenhum, e o estado da repetição passou a se chamar `JA_REGISTRADO`, com verde de uma piscada no lugar do vermelho. O pseudocódigo da seção 5 entregue ainda protege só os primeiros 5 s ("SE mesmo UID há menos de 5 s: estado = DUPLICADO"), e a figura da seção 4 faz o mesmo. A tabela da seção 6 continua valendo, porque o teste 1 pede um registro só e o teste 8 pede nenhuma duplicata.

O teste 4 da seção 6 diz "com o Iniciar sem internet", mas desde a R14 (06/09) o modo offline se decide no login, como mostra o pseudocódigo da própria seção 5. A spec passou a dizer "login sem internet".
