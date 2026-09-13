# Entrega 04 — Especificação e preparação para o desenvolvimento

> ✅ **Montado e entregue.** O documento que o professor recebeu está transcrito em `entrega-04.md`, com o PDF em `entrega-04.pdf`. Este rascunho guarda o texto integral, anterior aos cortes da montagem.
>
> **Espaço de montagem compartilhado**, no mesmo papel que o `entrega-03-rascunho.html` teve na entrega anterior. Cada seção traz de onde sai o conteúdo, quem escreve e até quando. A formatação em ABNT, a capa e a numeração são da montagem final (#13), que produz a transcrição em `entrega-04.md` ao lado do PDF.
>
> **Contrato do artefato.** Finalidade: mostrar ao professor o que o projeto especificou e preparou para o desenvolvimento. Público: o professor que avalia, não desenvolvedor. Registro: acadêmico direto, primeira pessoa do plural, sem nome de arquivo, de função nem de commit. Formato: PDF de 2 a 4 páginas, montado como **capa mais quatro de conteúdo**, o formato em que o professor aceitou a Entrega 03; sete seções, cinco delas ocupadas por diagrama, tabela, fluxograma, pseudocódigo e tabela; a seção 1 tem meia página, de 250 a 350 palavras. Fonte dos fatos: `spec.md` e `DIARIO.md`; fato que não está lá se pergunta, não se preenche.
>
> **Modalidade protegida.** Os Planos A e B de rádio seguem **em aberto**. Existem critérios de medição, não veredito, e nenhuma medição de bancada foi feita — nenhum verbo do documento pode apresentar como escolhido ou medido o que ainda é plano.
>
> ⚠️ **Este bloco de trabalho sai do documento na montagem.**

## 1 EVOLUÇÃO DESDE A ENTREGA ANTERIOR

*Origem: §2 da spec (rodadas 7 e 8), §9, §10 e a semana 4 do diário · Dono: Caio (#11) · Prazo: domingo, 13/09 · **escrita***

O obstáculo mais caro do período é físico. Há um microcontrolador só para cinco pessoas, e a divisão de responsabilidades de 07/09 descrevia cinco frentes com um dono cada, várias delas precisando da placa ao mesmo tempo. Em vez de o equipamento circular, ele ficou com o João, e são as tarefas que chegam até a placa prontas para executar, com o programa escrito e o passo a passo do que deve aparecer. Uma mudança em relação à Entrega 03: a tabela de responsabilidades descreve os papéis, e o que distribui o trabalho hoje são tarefas com dono e prazo.

Passamos a decidir por escrito, em tópicos abertos no repositório, e a primeira rodada decidida assim corrigiu um erro da nossa própria especificação. O João contestou a afirmação de que o microcontrolador não pode ser rede e cliente de internet ao mesmo tempo, e a documentação do fabricante confirma que o modo simultâneo é nativo. A decisão registrada em 05/09, de alternar entre os dois modos, partia de premissa errada e foi reaberta.

Com isso o principal desafio técnico mudou de forma. A pergunta deixou de ser como alternar entre os modos sem derrubar o professor e passou a ser qual dos dois arranjos o hardware sustenta, porque o impedimento que resta é de memória: a conexão segura com a nuvem exige de 40 a 50 KB livres no instante em que é aberta, e esse espaço precisa sobrar com a rede do aparelho, o servidor de páginas e a lista da turma de pé. Os dois entraram no plano de testes com critérios de aprovação.

No mesmo período o modelo de dados ganhou três entidades propostas pelo Cauã — o professor com cadastro próprio, a turma como tabela e a aula com estado, o mecanismo que impede o envio duplicado do relatório —, o Igor assumiu a tela de histórico, o João comprou todas as peças e a especificação recebeu as histórias de usuário, as ligações pino a pino, o pseudocódigo e a política de teste.

## 2 ARQUITETURA DO SISTEMA

*Origem: §3 da spec, imagem `diagrama-blocos.png`, exportada de `diagrama-blocos.drawio` · Dono: Caio (#13) · Prazo: segunda, 14/09 · **escrita***

> Imagem do diagrama de blocos.

Figura 1 – Diagrama de blocos. O rádio fala com o celular do professor e com o hotspot ao mesmo tempo ou alternando entre os dois; a escolha sai dos testes de bancada. A API encena o sistema acadêmico da UVA.

## 3 LIGAÇÕES DO ESP32

*Origem: §4.1 da spec, tabela `componente | GPIO | direção` · Dono: Caio (#13) · Prazo: segunda, 14/09 · **escrita***

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

O leitor trabalha estritamente em 3,3 V, fornecidos pelo regulador da placa, e a fonte de 5 V ligada à tomada alimenta o conjunto. Uma checagem de bancada vai dizer se o buzzer dispensa componente extra ou precisa de um transistor. A tabela traz um sensor só porque o professor dispensou este projeto da exigência de dois.

## 4 LÓGICA DO PROGRAMA

*Origem: fluxograma novo, em `fluxograma-logica.drawio` (R27) · Dono: Gabriel (#10) · Prazo: domingo, 13/09 · **escrita***

> Imagem do fluxograma, a 16 cm de largura.

Figura 2 – Fluxograma da lógica do programa.

## 5 PSEUDOCÓDIGO

*Origem: §5 da spec, na ordem e com os nomes do fluxograma da seção 4 · Dono: Caio (#13) · Prazo: segunda, 14/09 · **escrita***

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

*Origem: §8 da spec, com as lacunas conhecidas — os testes 7 e 7b, o arquivo de sessão e a máquina de estados completa. A observação sobre os testes 3, 4, 5 e 6 não é daqui: foi escrita na seção 7 · Dono: Caio (#13) · Prazo: segunda, 14/09 · **escrita***

Os testes são feitos na bancada, com o aparelho real, e verificam só o comportamento externo: o que o LED e o buzzer fazem, o que o portal mostra e o que sai no relatório exportado e no log serial. Cada teste deixa evidência gravada, em foto, vídeo, arquivo exportado ou trecho do log.

| # | Teste | Critério |
|---|---|---|
| 1 | Crachá de 4 bytes (Mifare), três aproximações seguidas | Um registro só, com resposta em menos de 200 ms |
| 2 | Crachá de 7 bytes (NTAG215) | Registra pelo UID, sem escrever na tag |
| 3 | Modo online, crachá fora da turma | Vermelho longo e nada gravado |
| 4 | Modo offline, com o Iniciar sem internet | Sessão marcada como offline, todo UID aceito e relatório íntegro |
| 5 | Enviar sem internet e, depois, com internet | O Compartilhar funciona, e a nova tentativa sobe o mesmo relatório sem duplicar |
| 6 | Dois professores em sequência | Duas sessões, cada uma com o seu professor, sem mistura |
| 7 | *Plano B* — alternância de rádio, 10 ciclos seguidos | A rede do aparelho volta nas 10 vezes, o celular reconecta e cada troca leva de 5 a 15 s |
| 7b | *Plano B* — falha forçada na volta à rede do aparelho, com o hotspot desligado no meio | O aparelho se recupera sozinho e não perde as presenças já registradas |
| 7c | *Plano A* — modo simultâneo, com o professor no portal e o aparelho conectando ao hotspot em outro canal | O painel continua aberto sem novo login, em iPhone e em Android, e, se cair, reconecta sozinho em menos de 5 s; com dois celulares do mesmo sistema, o veredito sai parcial |
| 7d | *Plano A* — memória livre no instante da conexão segura, com a rede do aparelho, o portal e a lista da turma carregados | Pelo menos 40 KB livres, e abaixo disso vale o Plano B; medido sem celular conectado ao aparelho, o número sai otimista, e de 40 a 45 KB é suspeito |
| 8 | Estresse: 30 toques, com um reinício no meio | Nenhuma duplicata e nenhuma perda de registro |

Os testes 7 e 7b só passam a valer se a bancada indicar o Plano B. A retomada da sessão depois de um reinício e a máquina de estados completa ainda não têm teste e entram no plano depois de 22/09.

## 7 PREPARAÇÃO PARA 21/09

*Origem: checklist novo, a partir da divisão em tarefas de 11/09 · Dono: Caio (#12) · Prazo: domingo, 13/09 · **escrita***

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
