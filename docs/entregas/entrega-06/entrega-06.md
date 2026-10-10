# Entrega 06 — Estrutura física do protótipo

> **Transcrição do documento entregue**, `Entrega 06 - 20261006 - Documento Estrutura Física do Protótipo.pdf` (21 páginas: capa, resumo, sumário e 18 de conteúdo, com as referências na última), arquivado em `entrega-06.pdf`. Postado no Teams pelos cinco integrantes até 06/10/2026, 23:59 (informação do Caio, 10/10).
>
> O documento foi montado pelo Gabriel (#56), e esta entrega não teve rascunho versionado no repositório. Esta transcrição existe para que o repositório registre o que o professor recebeu, palavra por palavra, com os erros do original. O que mudou depois da entrega está declarado ao pé, sem tocar no texto.

---

## Capa

> Logotipo da UVA centralizado no topo.

UNIVERSIDADE VEIGA DE ALMEIDA
Bacharelado em Ciência da Computação

GABRIEL ALBUQUERQUE VARELA SANTARELLO – 1240110815
CAUÃ MANUEL PROENÇA DE ANDRADE – 1240109764
IGOR ROCHA LOBATO – 1240114118
CAIO PARADA OLIVEIRA PLANINSCHEK – 1240205596
JOÃO VICTOR BERÇOT CHABUDET CABRAL – 1240108001

**PRESENTE!**
DISPOSITIVO EMBARCADO PARA CONTABILIZAÇÃO DE PRESENÇA EM SALA DE AULA

(Trabalho da disciplina de Sistemas Embarcados)
Entrega 06 – Estrutura física do protótipo

Professor: Thiago Alberto Ramos Gabriel

Rio de Janeiro RJ
2026

---

## Resumo

Este documento apresenta a estrutura física do Presente!, terminal embarcado que registra a presença do aluno em sala de aula pela aproximação do crachá, montado com o microcontrolador ESP32, o leitor RFID RC522, um LED RGB e um buzzer ativo. A estrutura é uma caixa de tênis de papelão reaproveitada, pensada para ficar presa na parede ao lado da porta, com a tampa voltada para a sala e a dobradiça em cima; o papelão custa quase nada e deixa passar tanto o sinal do leitor quanto o do Wi-Fi. O planejamento parte de três restrições do circuito: o leitor fica logo atrás da tampa, a antena do ESP32 fica livre de fios e de metal, e a tampa abre sem rasgar, para a manutenção e para gravar programa novo com a placa dentro da caixa. O documento reúne o desenho, feito antes da montagem e atualizado depois dela para mostrar a caixa construída, o modelo 3D no Tinkercad, as fotos da montagem de 05/10 e dos ajustes de 06/10, o material usado, as alterações em relação ao desenho, os problemas encontrados, com as soluções, e o teste com a caixa montada, conferido pela lista do enunciado. Com a tampa fechada, o leitor reconheceu o cartão em todas as passadas gravadas em 05/10, o buzzer apitou nas seis leituras dos vídeos, e a placa pôs no ar a própria rede Wi-Fi. Em 06/10, o ESP32, que tinha ficado solto e foi a causa provável de uma parada do leitor durante a montagem, foi preso no fundo da caixa com quatro parafusos M2,5 e arruelas, e as etiquetas do buzzer e do ESP32 foram coladas. A caixa fica apoiada na mesa até a demonstração de 27/10, quando vai para a parede, e o furo e a etiqueta do LED ficam para a aula de 06/10.

Palavras-chave: estrutura física; ESP32; RFID; registro de presença; protótipo.

## Sumário

> Sumário com o número da página de cada seção, no original.

1 FOTO DO PROTÓTIPO COMPLETO (3) · 1.1 Como o aparelho é usado na sala (3) · 1.2 O que a estrutura contém (4)
2 FOTOS DO PROCESSO DE CONSTRUÇÃO (5) · 2.1 O ponto de partida (5) · 2.2 A montagem de 05/10 (6) · 2.3 Os ajustes de 06/10 (6)
3 PLANEJAMENTO: O DESENHO E O MODELO NO TINKERCAD (8) · 3.1 O que o circuito exige da caixa (8) · 3.2 A caixa escolhida (8) · 3.3 O desenho (9) · 3.4 O modelo no Tinkercad (11)
4 VÍDEO E TESTE COM A CAIXA FECHADA (12) · 4.1 O vídeo (12) · 4.2 O teste (13)
5 MATERIAIS USADOS (14)
6 ALTERAÇÕES EM RELAÇÃO AO DESENHO (15)
7 PROBLEMAS ENCONTRADOS E SOLUÇÕES (17)
8 ATUALIZAÇÃO DO DIÁRIO DE BORDO (19)
REFERÊNCIAS (20)

---

## 1 FOTO DO PROTÓTIPO COMPLETO

> Fotografia da caixa fechada, de frente, a 16 cm de largura, com três chamadas: "Tampa de papelão: é a frente do aparelho, e o crachá é lido através dela"; "Etiqueta "Encoste o crachá aqui": o leitor RC522 fica logo atrás dela, por dentro da tampa"; e "Etiqueta Buzzer, colada em 06/10, acima dos quatro furos para o som do buzzer, num quadrado de 2 por 2". O monitor ao fundo aparece borrado. A foto é a de `docs/assets/estrutura/caixa-montada-fechada-2026-10-06.jpg`.

**Figura 1** – O Presente! montado na caixa, com a tampa fechada, em 06/10.

Fonte: elaborado pelos autores (2026).

O Presente! fica dentro de uma caixa de tênis de papelão, deitada, com a tampa virada para a frente. Na tampa estão a etiqueta "Encoste o crachá aqui", logo à frente do leitor de crachá, que fica por dentro, e, à esquerda dela, os quatro furos do som do buzzer, com a etiqueta Buzzer acima. A foto é de 06/10, depois de prendermos o ESP32 no fundo da caixa com parafusos e de colarmos as etiquetas do buzzer e do ESP32 (seção 2.3); a montagem e o primeiro teste com a caixa fechada foram na noite anterior, 05/10 (seções 2 e 4). Por enquanto, a caixa fica apoiada na mesa, e vai para a parede na demonstração de 27/10 (seção 7). O furo do LED, à direita da etiqueta do crachá, e a etiqueta dele ficam para a aula de 06/10; as seções 6 e 7 explicam esses pontos.

### 1.1 Como o aparelho é usado na sala

O enunciado pergunta como o sistema seria construído para ser usado numa situação real. Na sala, o aparelho fica preso na parede, ao lado da porta, ligado na tomada. Ao entrar, o aluno encosta o crachá na etiqueta da tampa; o leitor, logo atrás dela, lê o número do crachá, e o LED e o bipe confirmam a presença sem que ele precise olhar para uma tela. O professor abre e fecha a chamada pelo celular, conectado à rede Wi-Fi do próprio aparelho, e no fim da aula o relatório sobe para a nuvem, ou vai para o celular do professor, se faltar internet. Por isso a caixa precisa deixar passar dois sinais, o do leitor, pela frente, e o do Wi-Fi, por onde o celular do professor se conecta, e precisa abrir para manutenção sem ser tirada da parede.

Esse é o uso para o qual o aparelho foi projetado. Nesta etapa, a caixa recebe o protótipo que lê o crachá e dá o aviso; a chamada pelo celular entra com o programa completo, ainda em desenvolvimento.

### 1.2 O que a estrutura contém

O enunciado lista o que a versão física deve conter. O Quadro 1 mostra onde cada item fica na caixa construída, como no desenho atualizado (Figura 6), e o estado de cada um em 06/10.

**Quadro 1** – Os elementos que o enunciado pede e onde estão na caixa.

| Elemento | Onde fica | Estado em 06/10 |
|---|---|---|
| Microcontrolador | ESP32 DevKit V1, sem placa de expansão, deitado na parede do fundo, com o USB-C para a direita | Preso por quatro parafusos M2,5 com arruelas; em 05/10, estava solto (seção 7) |
| Sensor | Leitor RC522, de pé sobre a protoboard, com a face da antena voltada para a tampa | Leu o crachá através da tampa em 05/10 |
| Atuadores | Buzzer ativo de 5 V, à esquerda do leitor, e LED RGB, à direita, os dois na protoboard | O buzzer tem quatro furos na tampa; o furo do LED fica para a aula de 06/10 |
| Estrutura de suporte | Caixa de tênis de papelão, deitada, com a tampa na frente e a dobradiça em cima | Montada; apoiada na mesa até a demonstração de 27/10, quando vai para a parede |
| Posicionamento do sensor | Logo atrás da tampa, de cerca de 2 mm, na altura da etiqueta "Encoste o crachá aqui", sem metal na frente da antena | Leitura confirmada com a tampa fechada em 05/10 |
| Localização dos atuadores | Na frente, dos dois lados da etiqueta, onde o aluno olha e escuta ao encostar o crachá | O bipe se ouve com a tampa fechada; o verde ainda só aparece com ela aberta |
| Alimentação e conexões | Cabo USB-C no ESP32, saindo por um furo na parede de baixo em direção à tomada; jumpers entre o ESP32 e a protoboard | Ligações do leitor refeitas na montagem; etiqueta Energia colada por fora |
| Espaço para os componentes | 33 × 20 × 12 cm por dentro, para a protoboard de 16,6 × 5,5 cm e a placa de cerca de 2,8 × 5,2 cm | Sobra espaço em todas as direções |
| Identificação | Etiquetas "Encoste o crachá aqui", Energia, Buzzer, ESP32 e LED | Quatro coladas; a do LED fica para a aula de 06/10, com o furo |

Fonte: elaborado pelos autores (2026).

## 2 FOTOS DO PROCESSO DE CONSTRUÇÃO

Montamos a caixa na noite de segunda, 05/10, na faculdade, com a placa que o João levou, e os cinco integrantes participaram: Caio, Cauã, Gabriel, Igor e João. A caixa foi planejada ao longo das duas semanas anteriores, e o Quadro 2 mostra as datas.

**Quadro 2** – Do planejamento à montagem.

| Data | O que aconteceu |
|---|---|
| 22/09 | O protótipo eletrônico é aprovado na demonstração, e o professor adia a estrutura física para 06/10 |
| 23/09 | Decidimos fazer a caixa de papelão ou de material reaproveitado, com custo perto de zero |
| 29/09 | Chega o enunciado; o desenho passa a vir antes da montagem, e o Caio propõe a ideia base da caixa (#31) |
| 30/09 | A caixa de tênis do Caio é medida, e o desenho é refeito com ela (#34); o João já tinha comprado o buzzer novo e os cartões (#39) |
| 01/10 | Sem outra proposta, a ideia base é escolhida (#31) |
| 05/10 | Montagem e teste de leitura com a tampa fechada, na faculdade, com os cinco integrantes (#32, #33) |
| 06/10 | O ESP32 é preso no fundo com quatro parafusos M2,5 e arruelas, as etiquetas do buzzer e do ESP32 são coladas, e o desenho passa a mostrar a caixa construída (#34); o Igor publica o modelo no Tinkercad com as mudanças da montagem (#57) |
| 27/10 | Demonstração no laboratório, com a caixa presa na parede (previsto) |

Fonte: elaborado pelos autores (2026).

### 2.1 O ponto de partida

> Fotografia do protótipo de 22/09 sobre a mesa, a 16 cm de largura, com seis chamadas: "LED RGB, na outra ponta"; "Leitor RC522, de pé na protoboard"; "Transistor TIP122"; "Buzzer, numa ponta da protoboard (a peça antiga, que só estalava)"; "Protoboard de 830 pontos"; e "ESP32, ligado à protoboard por jumpers". A foto é a de `docs/assets/testes/prototipo-montado-2026-09-22.jpg`.

**Figura 2** – O ponto de partida: o protótipo aprovado em 22/09, que a caixa recebe.

Fonte: elaborado pelos autores (2026).

A caixa foi pensada para receber o protótipo da entrega anterior sem desmontá-lo (Figura 2). Nele, o leitor fica de pé na protoboard, o LED fica numa ponta dela, e o buzzer e o transistor que o aciona, na outra; o ESP32 fica fora da protoboard, ligado a ela por jumpers. Por isso a ideia base deixa o leitor de pé, como está, em vez de colá-lo na tampa, e a tampa sai sem puxar fio nenhum. A peça que mudou até a montagem foi o buzzer: o da Figura 2 só estalava, e o João comprou um buzzer ativo novo (seção 7).

### 2.2 A montagem de 05/10

> Três fotografias lado a lado, marcadas (a), (b) e (c), a 16 cm de largura no conjunto. São as de `docs/assets/estrutura/fixando-os-pinos-na-protoboard-2026-10-05.jpg`, `montando-a-protoboard-na-caixa-1-2026-10-05.jpg` e `montando-a-protoboard-na-caixa-2-2026-10-05.jpg`.

**Figura 3** – Três momentos da montagem: (a) a protoboard, com o leitor de pé e o ESP32 preso aos fios, sai da mesa e vai para a caixa, às 20h45, pelo relógio do celular que aparece na foto; (b) com a protoboard já na caixa, os integrantes mexem nos fios do leitor e do ESP32; (c) a caixa aberta, durante os ajustes.

Fonte: elaborado pelos autores (2026).

A protoboard foi para o lugar previsto no desenho, deitada na parede de baixo da caixa e presa com fita crepe, com o leitor de pé sobre ela, voltado para a tampa. Quando as peças entraram na caixa, o leitor parou de responder, e refizemos todas as ligações dele, seguindo as anotações, até ele voltar a ler. Também furamos a tampa para o som do buzzer e testamos a leitura com a tampa fechada (seção 4). O ESP32 ficou solto dentro da caixa, ligado pelos fios, porque descartamos a fita dupla-face na placa, e foi preso no dia seguinte (seção 2.3). As fotos e os vídeos da montagem estão na pasta docs/assets/estrutura/ do repositório do projeto.

### 2.3 Os ajustes de 06/10

No dia seguinte, 06/10, prendemos o ESP32 na parede do fundo com quatro parafusos M2,5, um em cada canto da placa, e colamos mais duas etiquetas: a do ESP32, por dentro, no fundo, acima da placa, e a do buzzer, na tampa, logo acima dos quatro furos do som (Figuras 4 e 5). A placa ficou deitada, com o USB-C para a direita, e arruelas entre ela e o papelão erguem o lado do USB-C, para que o plugue do cabo encaixe; a seção 7 descreve essa montagem. A caixa ficou apoiada na mesa, onde fica até a demonstração de 27/10.

> Fotografia da caixa aberta, com a tampa levantada, a 16 cm de largura, com seis chamadas: "Tampa levantada: a dobradiça fica em cima"; "Etiqueta ESP32, colada no fundo"; "ESP32 deitado, preso no fundo por quatro parafusos M2,5, com o USB-C para a direita"; "LED, ainda sem caminho até a tampa"; "Protoboard presa com fita crepe na parede de baixo"; e "Transistor TIP122, na ponta do buzzer". A foto é a de `docs/assets/estrutura/caixa-montada-aberta-2026-10-06.jpg`.

**Figura 4** – A caixa aberta em 06/10, com as peças identificadas, já com o ESP32 preso no fundo. O leitor RC522 não aparece: por ser a peça mais sensível, ficou guardado com o João e volta para a mesma posição na protoboard.

Fonte: elaborado pelos autores (2026).

> Dois recortes lado a lado, marcados (a) e (b), a 16 cm de largura no conjunto: (a) o ESP32 parafusado sob a etiqueta "ESP32"; (b) a etiqueta "Buzzer" sobre os quatro furos, ao lado da etiqueta "Encoste o crachá aqui".

**Figura 5** – Os ajustes de 06/10 em detalhe: (a) o ESP32 preso na parede do fundo com quatro parafusos, um em cada canto, abaixo da etiqueta ESP32; (b) a etiqueta do buzzer colada acima dos quatro furos do som, ao lado da etiqueta "Encoste o crachá aqui".

Fonte: elaborado pelos autores (2026).

A etiqueta do LED, já impressa, e o furo dele ficam para a aula de 06/10. A foto da caixa fechada desse dia é a Figura 1, e as duas fotos aparecem também na Figura 10, ao lado do desenho de 30/09 (seção 6); elas estão na pasta docs/assets/estrutura/, com as de 05/10.

## 3 PLANEJAMENTO: O DESENHO E O MODELO NO TINKERCAD

### 3.1 O que o circuito exige da caixa

O planejamento parte de três restrições do circuito. O leitor RC522 só alcança poucos centímetros, por isso a face dele, o lado com a antena desenhada, fica paralela à tampa da frente e o mais perto possível dela, numa parte da tampa com no máximo 2 a 3 mm de espessura e sem metal por perto. A ponta do ESP32 onde fica a antena do Wi-Fi permanece livre de fios e de metal, porque é por ela que o celular do professor se conecta ao aparelho. E a tampa precisa abrir sem rasgar, porque o programa completo vai ser gravado com a placa já dentro da caixa, apertando o botão BOOT, e porque é por ela que se faz a manutenção.

### 3.2 A caixa escolhida

Escolhemos uma caixa de papelão reaproveitada, porque o custo precisava ficar perto de zero e porque o papelão deixa passar tanto o sinal do leitor quanto o do Wi-Fi. É uma caixa de tênis, com 33 × 20 × 12 cm por dentro e 35,3 × 21,7 × 12,6 cm por fora, e uma tampa presa num dos lados compridos, que fecha com uma aba de 4,5 cm por fora. Na parede, ela fica deitada: a tampa vira a frente, com a dobradiça no lado comprido de cima, e o fundo da caixa vai contra a parede da sala, preso com fita dupla-face. Assim o peso da tampa a mantém fechada, e ela abre levantando por baixo. Por enquanto, a caixa fica apoiada na mesa, na mesma posição, e vai para a parede na demonstração de 27/10 (seção 7).

Por dentro, a protoboard fica deitada na parede de baixo, encostada na tampa, com o leitor de pé sobre ela, como na montagem de 22/09, e o ESP32 fica na parede do fundo; o cabo de energia sai por um furo na parede de baixo. A tampa tem cerca de 2 mm de papelão, espessura medida pelo Caio em 05/10 e dentro do limite do leitor. No desenho de 30/09, a tampa teria o furo do LED à esquerda do leitor e nove furos para o som do buzzer à direita, e o ESP32 ficaria de pé, numa placa de expansão, com o USB para baixo; o que mudou na montagem está na seção 6. A caixa leva cinco etiquetas, "Encoste o crachá aqui", Buzzer, LED, ESP32 e Energia, com os mesmos nomes do desenho e da apresentação. Antes de chegar a essa caixa, descartamos outras opções, e o Quadro 3 resume o motivo de cada uma.

**Quadro 3** – Opções que consideramos e por que ficaram de fora.

| Opção | Por que ficou de fora |
|---|---|
| Caixa impressa em 3D | Ninguém do grupo modela em CAD, e não havia impressora disponível |
| MDF cortado em papelaria, com parafusos | Custa e exige ferramenta, e o papelão atende |
| Leitor colado atrás da tampa | Os fios dele seriam puxados toda vez que a tampa abrisse |
| Dobradiça de lado | A tampa abriria como uma porta, sem nada que a mantivesse fechada |
| Dobradiça embaixo | A tampa cairia aberta com o próprio peso |

Fonte: elaborado pelos autores (2026).

### 3.3 O desenho

O desenho foi feito no draw.io antes da montagem, com as medidas da caixa real, tomadas em 30/09, para que a entrega mostre o que planejamos ao lado do que construímos. Depois da montagem, em 06/10, o Caio o atualizou para mostrar a caixa como foi construída: o buzzer à esquerda do leitor, com quatro furos, o LED à direita, o ESP32 deitado, sem placa de expansão e preso por parafusos, e a tampa com cerca de 2 mm. A versão de 30/09, o *como imaginamos*, continua no histórico do repositório e na issue #34, e aparece na Figura 10, ao lado das fotos da caixa montada (seção 6). O desenho atualizado está dividido em duas figuras, para que as anotações fiquem legíveis: a Figura 6 mostra a caixa de frente, com a tampa fechada, e por dentro, com a tampa levantada, e a Figura 7, de lado, em corte. O arquivo inteiro, com a lista do que mudou em relação ao primeiro desenho e a das medidas, está no repositório, em docs/estrutura-fisica.png.

> Primeira parte do desenho atualizado em 06/10, a 15,5 cm de largura: a vista de frente, com a tampa fechada, e a vista por dentro, com a tampa levantada. Recorte de `docs/estrutura-fisica.png`.

**Figura 6** – Desenho da caixa como foi construída, atualizado em 06/10 (parte 1): de frente, com a tampa fechada, e por dentro, com a tampa levantada.

Fonte: elaborado pelos autores (2026).

> Segunda parte do desenho, a 13,5 cm de largura: a vista de lado, em corte, com a tampa levantada. Recorte de `docs/estrutura-fisica.png`.

**Figura 7** – Desenho da caixa como foi construída (parte 2): de lado, em corte, com a tampa levantada.

Fonte: elaborado pelos autores (2026).

### 3.4 O modelo no Tinkercad

Fora do enunciado, o professor pediu também um modelo em três dimensões da caixa, feito no Tinkercad. O Igor montou o modelo com as medidas do desenho: o corpo, com 330 × 120 × 200 mm por dentro e paredes de 3 mm; a tampa, com a aba de 4,5 cm; o furo de 15 mm do cabo na parede de baixo; a protoboard, o leitor, o LED, o buzzer e o ESP32 por dentro; e as cinco etiquetas em relevo. O modelo é da caixa, e não do circuito, porque o simulador de circuitos do Tinkercad não tem o ESP32.

Depois da montagem, o Igor atualizou o modelo para a caixa montada: o buzzer à esquerda do leitor, com quatro furos, o LED à direita, e o ESP32 sem placa de expansão (seção 6). O modelo mostra onde cada peça fica, e não como ela está presa; por isso os parafusos do ESP32, de 06/10, não aparecem nele. O leitor e o ESP32 aparecem com a aparência real, importados de modelos 3D gratuitos e convertidos para o formato que o Tinkercad aceita. O modelo está publicado em tinkercad.com/things/kSh8mDZEolG, e o código QR da Figura 9 leva até ele.

Os dois modelos importados têm licença Creative Commons, que pede o crédito aos autores. O leitor RC522 é o modelo "RFID read/write module RC522", de Stichting Consortium Beroepsonderwijs (2022), publicado no Sketchfab sob a licença CC BY, e o ESP32 é o modelo "ESP32 DEVKITV1", de Marek38 (2024), publicado no MakerWorld sob a licença CC BY-NC-SA. As duas referências completas estão no fim do documento.

## 4 VÍDEO E TESTE COM A CAIXA FECHADA

### 4.1 O vídeo

Gravamos três vídeos curtos na noite de 05/10: dois com a tampa fechada, um de frente e um de cima, e um com a caixa aberta, mostrando a placa. O vídeo da entrega junta os três, com 28 segundos e legendas, na ordem que o enunciado pede (Figura 8). Ele começa pela caixa fechada, com a etiqueta e os furos do buzzer, que é a estrutura física. Depois, o cartão encosta na tampa, em frente ao leitor, que é o sensor. Com a caixa aberta, aparecem o leitor e o ESP32, que faz o processamento: recebe o número do cartão e decide a resposta. A cada leitura, o atuador responde com duas piscadas verdes e um bipe. Por último vem o resultado: com a tampa fechada, cada passada do cartão é reconhecida com um bipe.

> Cinco quadros do vídeo lado a lado, a 15 cm de largura, cada um com uma legenda: "1 · Estrutura física / a caixa fechada / no vídeo: 0:01"; "2 · Sensor / o cartão na tampa / no vídeo: 0:07"; "3 · Processamento / o ESP32 e o leitor / no vídeo: 0:10"; "4 · Atuador / o verde e o bipe / no vídeo: 0:13"; "5 · Resultado / lido pela tampa / no vídeo: 0:23".

**Figura 8** – O vídeo da entrega quadro a quadro, nas cinco etapas que o enunciado pede, com o momento de cada uma no vídeo.

Fonte: elaborado pelos autores (2026).

Medimos nos próprios vídeos o que o aparelho faz em cada leitura. O bipe aparece no áudio como um tom de cerca de 2,6 kHz, com pouco mais de um décimo de segundo, e as duas piscadas verdes duram cerca de 0,1 s cada, com 0,1 s de intervalo. É o padrão que o programa do sinal de presença usa para o crachá registrado: duas piscadas de 90 ms e um bipe de 100 ms. O bipe e a primeira piscada começam praticamente juntos. Nas seis leituras dos três vídeos, o bipe soou todas as vezes, e, com a tampa fechada, o verde não aparece em nenhum quadro, porque o LED ainda não tem furo na tampa.

O vídeo está no repositório, com os três vídeos originais, e o código QR da Figura 9 abre o vídeo caso ele não abra no Teams.

> Três códigos QR lado a lado, a 10 cm de largura, com as legendas "Vídeo da entrega / 28 s, no repositório", "Modelo 3D no Tinkercad / tinkercad.com/things/kSh8mDZEolG" e "Repositório do projeto / github.com/caioplaninschek/presente".

**Figura 9** – Códigos QR para o vídeo da entrega, o modelo no Tinkercad e o repositório do projeto.

Fonte: elaborado pelos autores (2026).

### 4.2 O teste

Com a caixa montada, conferimos a lista que o enunciado pede para a estrutura pronta (Quadro 4). Esse é o teste de 05/10, com o ESP32 ainda solto; o completo, com a placa presa, é o da aula de 06/10 (#33). O risco principal era o papelão da tampa entre o crachá e o leitor, e ele não impediu a leitura: nas três leituras feitas com a tampa fechada, o bipe soou. Com as peças dentro da caixa, a placa também pôs no ar a própria rede Wi-Fi. As respostas "em parte" e "não" do quadro vêm do ESP32 solto, que foi preso em 06/10, depois deste teste (seção 7).

**Quadro 4** – O teste de 05/10, com o ESP32 ainda solto, na lista do enunciado.

| O que o enunciado pede verificar | Resultado em 05/10 |
|---|---|
| Se os sensores leem | Sim, através da tampa: nas três leituras feitas com ela fechada, houve bipe. Os vídeos mostram só o cartão; o chaveiro não foi testado com a caixa fechada |
| Se o atuador funciona | Sim. O bipe soou nas seis leituras dos vídeos, e o LED piscou verde a cada leitura; com a tampa fechada, o verde ainda não aparece, porque o LED não tem furo |
| Se os componentes estão firmes | Em parte. A protoboard ficou presa com fita crepe; o ESP32 ficou solto, pendurado nos fios |
| Se os fios estão organizados | Não. Os fios ficaram soltos dentro da caixa, e o ESP32 pendurado neles é a causa provável de o leitor ter parado na montagem |
| Se o microcontrolador está protegido | Em parte. Ele fica dentro da caixa fechada, mas solto |
| Se dá para chegar à alimentação e ao USB | Com a tampa aberta, sim. Ligar e desligar o cabo sem abrir a caixa não foi verificado |
| Se a estrutura permite manutenção e ajustes | Sim. A tampa abre e fecha pela dobradiça da própria caixa, sem rasgar, e foi aberta e fechada durante a montagem e os testes. Gravar programa novo com a placa dentro da caixa não foi testado |
| Se o protótipo cumpre a função | Na parte do aluno, sim: o crachá é reconhecido através da tampa, com aviso sonoro. A rede Wi-Fi do aparelho subiu com as peças na caixa; abrir o painel do professor no celular com a caixa fechada não foi registrado |

Fonte: elaborado pelos autores (2026).

## 5 MATERIAIS USADOS

**Quadro 5** – Materiais da estrutura e do circuito que ela recebe.

| Material | Quantidade e medidas | Uso | Origem |
|---|---|---|---|
| Caixa de tênis de papelão | 1; 33 × 20 × 12 cm por dentro e 35,3 × 21,7 × 12,6 cm por fora | Corpo e tampa da frente | Reaproveitada, do Caio |
| Fita crepe | O necessário | Prender a protoboard na parede de baixo | Do grupo |
| Etiquetas de papel impressas | 5; 4 coladas, e a do LED fica para a aula de 06/10 | Identificação dos elementos | Preparadas pelo Caio |
| Parafusos M2,5, porcas e arruelas | 4 parafusos, 4 porcas e 12 arruelas | Prender o ESP32 no fundo: 4 arruelas em cada parafuso do lado do USB-C e 2 em cada um dos outros | Do grupo |
| ESP32 DevKit V1 | 1; cerca de 2,8 × 5,2 cm | Processamento e Wi-Fi | Do protótipo |
| Leitor RC522, de 13,56 MHz | 1; cerca de 6 × 4 cm | Sensor | Do protótipo |
| LED RGB de catodo comum | 1, com 3 resistores de 300 Ω | Aviso visual | Do protótipo |
| Buzzer ativo de 5 V e transistor TIP122 | 1 de cada | Aviso sonoro | Buzzer comprado antes da montagem |
| Protoboard e jumpers | 1 protoboard de 830 pontos, com 16,6 × 5,5 × 1,0 cm | Ligações | Do protótipo |
| Cabo USB-C | 1 | Alimentação e gravação do ESP32 | Do protótipo |

Fonte: elaborado pelos autores (2026).

A caixa é reaproveitada e não custou nada. A compra do buzzer novo, feita pelo João junto com a dos cartões de crachá dos próximos testes, saiu por R$ 6,80 para cada integrante, R$ 34,00 no total.

## 6 ALTERAÇÕES EM RELAÇÃO AO DESENHO

A Figura 10 põe lado a lado o desenho de 30/09, o *como imaginamos*, e a caixa montada, nas fotos de 06/10, de frente e por dentro, e o Quadro 6, mais adiante, resume cada diferença. Depois da montagem, o desenho foi atualizado para mostrar a caixa construída (Figuras 6 e 7), e por isso a comparação usa a versão de 30/09.

> Montagem a 16 cm de largura, em duas colunas: à esquerda, sob o título "Como imaginamos: o desenho de 30/09", as vistas de frente e por dentro do desenho daquela data; à direita, sob o título "Como construímos: a caixa em 06/10", as fotos da caixa fechada e da caixa aberta, as mesmas das Figuras 1 e 4.

**Figura 10** – Como imaginamos e como construímos: o desenho de 30/09, à esquerda, e a caixa montada, à direita, nas fotos de 06/10, de frente e por dentro. Na foto de dentro, o leitor RC522 não aparece, porque ficou guardado com o João.

Fonte: elaborado pelos autores (2026).

O buzzer e o LED trocaram de lado. No desenho, o LED ficava à esquerda do leitor, e o buzzer, à direita; na montagem, por engano, os dois ficaram ao contrário. Como o LED fica numa ponta da protoboard e o buzzer na outra (Figura 2), basta a protoboard entrar na caixa no sentido inverso para os dois trocarem de lado. Os furos do som foram feitos onde o buzzer ficou, à esquerda da etiqueta, e são quatro, num quadrado de 2 por 2, e não os nove do desenho, que ficariam grandes e exagerados.

A altura dos furos saiu como o modelo previa. O Igor observou no Tinkercad que os furos ficariam baixos na tampa, a cerca de 2 cm da base, porque o LED e o buzzer estão sobre a protoboard deitada na parede de baixo (#57), e é isso que se vê na caixa montada (Figura 1).

**Quadro 6** – O que mudou do desenho de 30/09 para a caixa montada.

| Item | No desenho de 30/09 | Na caixa montada | Motivo |
|---|---|---|---|
| Protoboard e leitor | Protoboard deitada na parede de baixo, encostada na tampa; leitor de pé, voltado para ela | Como no desenho, com a protoboard presa por fita crepe | Sem mudança |
| Lado do buzzer e do LED | Buzzer à direita do leitor, e LED à esquerda | Buzzer à esquerda, e LED à direita | Engano na montagem |
| Furos do buzzer | Nove, num quadrado de 3 por 3 | Quatro, num quadrado de 2 por 2 | Nove ficariam grandes e exagerados |
| Furo do LED | Um furo de 5 mm na tampa | Ainda sem furo; ele fica para a aula de 06/10, com o LED puxado por quatro jumpers | O LED aponta para cima na protoboard e precisa de jumpers até a tampa |
| ESP32 | De pé no fundo, numa placa de expansão, com o USB para baixo e a antena para cima | Deitado no fundo, sem placa de expansão, com o USB-C para a direita, preso por quatro parafusos M2,5 com arruelas | Os fios vão direto nos pinos, e a placa não leva fita, porque esquenta; as arruelas erguem o lado do USB-C para o plugue encaixar |
| Lugar da caixa | Na parede, ao lado da porta | Na mesa até a demonstração de 27/10; depois, na parede, com calços de papelão nas quinas de fora do fundo | As porcas e as pontas dos parafusos saem pelo fundo |
| Tampa | Cerca de 1,5 mm, estimada pela foto | Cerca de 2 mm, medida em 05/10 | A medida substituiu a estimativa; a tampa continua dentro do limite de 2 a 3 mm do leitor |
| Etiquetas | Cinco | Quatro coladas; a do LED fica para a aula de 06/10 | A etiqueta do LED entra com o furo dele |

Fonte: elaborado pelos autores (2026).

O ESP32 não está numa placa de expansão, como o desenho de 30/09 supunha: as fotos mostram os fios ligados direto nos pinos dele. Ele também não foi colado na parede do fundo, porque descartamos a fita dupla-face na placa: por sugestão do João, a placa esquenta, e a cola ficaria grudada nela. Na montagem, ele ficou solto, e, em 06/10, foi preso no fundo com quatro parafusos, deitado, com o USB-C para a direita (seção 7). O LED ainda não tem furo na tampa, porque aponta para cima na protoboard; o furo e a etiqueta dele ficam para a aula de 06/10.

Antes da montagem, o próprio desenho já tinha mudado duas vezes. O leitor, que na primeira ideia ficava colado atrás da tampa, passou a ficar de pé na protoboard, como na foto de 22/09, e as medidas estimadas deram lugar às da caixa real, medida em 30/09, com a dobradiça em cima (Quadro 3). Depois da montagem, em 06/10, ele mudou de novo e passou a mostrar a caixa construída (seção 3.3).

## 7 PROBLEMAS ENCONTRADOS E SOLUÇÕES

**O buzzer só estalava.** No protótipo de 22/09, o buzzer dava um clique em vez do bipe. Para que o som fosse testado na caixa com a peça definitiva, e para não ter de abrir a caixa depois para trocá-la, o João comprou um buzzer ativo de 5 V antes da montagem. Nos vídeos de 05/10, o buzzer apita a cada leitura, o que a peça antiga não fazia.

**A montagem mudou de data.** Ela seria feita no fim de semana pelo João, que está com a placa, mas ele viajou, e a montagem passou para a segunda, na faculdade, com os cinco integrantes.

**O leitor parou de responder quando as peças entraram na caixa.** Refizemos todas as ligações dele, seguindo as anotações, até ele voltar a ler. A causa provável, ainda não confirmada, é o ESP32, que ficou solto, pendurado nos fios, e pode ter puxado algum deles. A solução definitiva é prender a placa, que é o problema seguinte.

**O ESP32 não pode ser colado.** Com a fita dupla-face descartada, o Caio planejou prender a placa com quatro parafusos M2,5, de 2,5 mm de diâmetro, um em cada canto, atravessando a parede do fundo da caixa, com um apoio de porcas entre a placa e o papelão. Em 06/10, prendemos a placa assim, mas com arruelas no lugar das porcas (Figura 11). Na ordem, vão a cabeça do parafuso, a placa, as arruelas, o fundo da caixa e a porca, por fora, de onde sai a ponta do parafuso. Os dois parafusos do lado do USB-C levam quatro arruelas cada um, e os outros dois, duas: esse lado fica mais alto, e a placa, levemente inclinada, para que o plugue do cabo encaixe sem bater no papelão. A placa ficou deitada no fundo, com o USB-C para a direita, e não leva fita. Foram usados 4 parafusos, 4 porcas e 12 arruelas, e as opções de reserva do plano, abraçadeiras ou um bolso de papelão, não foram precisas.

> Desenho da fixação vista de lado, fora de escala, a 15 cm de largura. Imagem em `docs/fixacao-esp32.png`.

**Figura 11** – A fixação do ESP32, vista de lado e fora de escala.

Fonte: elaborado pelos autores (2026).

**As porcas saem pelo fundo.** As porcas e as pontas dos parafusos ficam do lado de fora do fundo e impediriam a caixa de encostar reta na parede. Por isso, por enquanto, ela fica apoiada na mesa, como na aula de 06/10. Na demonstração de 27/10, ela vai para a parede, com calços de papelão nas quinas de fora do fundo e a fita dupla-face colada neles.

**O LED não alcança a tampa.** Ele aponta para cima na protoboard, e a tampa fica na frente. O plano é estendê-lo por quatro jumpers macho-fêmea até um furo de 5 mm na tampa, do lado direito, que é onde ele ficou na protoboard, com os resistores na própria protoboard. O furo e a etiqueta do LED ficam para a aula de 06/10, com o grupo.

## 8 ATUALIZAÇÃO DO DIÁRIO DE BORDO

O diário de bordo fica no arquivo DIARIO.md do repositório público do projeto, que o código QR da Figura 9 abre, e é atualizado a cada semana. A entrada desta semana registra a escolha da caixa, o desenho feito antes da montagem e o modelo no Tinkercad, a montagem de 05/10 com as fotos de cada etapa, o material, as alterações, os problemas com as soluções, o teste com a caixa fechada e os ajustes de 06/10: o ESP32 preso com parafusos e arruelas, as etiquetas coladas e o desenho atualizado para a caixa construída. O diário registra também quem fez cada parte da entrega (Quadro 7).

**Quadro 7** – Quem fez cada parte desta entrega.

| Integrante | Responsável por |
|---|---|
| Caio | A ideia base e o desenho da caixa (#31, #34), a caixa e as etiquetas, a organização da montagem (#32), o plano para prender o ESP32, o desenho da caixa construída e a figura da fixação |
| Cauã | O teste com a caixa fechada e o vídeo (#33) |
| Gabriel | Este documento e o diário de bordo (#56) |
| Igor | O modelo 3D no Tinkercad, com os modelos reais do leitor e do ESP32 (#57) |
| João | A placa e as peças do protótipo, a compra do buzzer e dos cartões (#39) e a sugestão de não colar o ESP32 |
| Os cinco | A montagem e o teste, na noite de 05/10 |

Fonte: elaborado pelos autores (2026).

## REFERÊNCIAS

MAREK38. **ESP32 DEVKITV1**. [*S. l.*]: MakerWorld, 30 mar. 2024. Modelo 3D. Licença Creative Commons Attribution-NonCommercial-ShareAlike (CC BY-NC-SA). Disponível em: https://makerworld.com/en/models/403304-esp32-devkitv1. Acesso em: 6 out. 2026.

STICHTING CONSORTIUM BEROEPSONDERWIJS. **RFID read/write module RC522**. [*S. l.*]: Sketchfab, 25 out. 2022. Modelo 3D. Licença Creative Commons Attribution (CC BY). Disponível em: https://sketchfab.com/3d-models/rfid-readwrite-module-rc522-09a7fab0dd574bd1bbaa267e78ffd996. Acesso em: 6 out. 2026.

---

## Divergência declarada depois da entrega (10/10)

O documento entregue fica como está. Estes pontos dele já não batem com o estado do projeto, registrado na spec (R61 a R64), no `DIARIO.md` e nos desenhos:

- **O furo e a etiqueta do LED.** O documento diz, no resumo e nas seções 1, 2.3, 5, 6 e 7, que eles ficam "para a aula de 06/10". Não foram feitos nessa aula. Por decisão do Caio, viraram uma tarefa própria (#58), com o João, para a aula de terça, 13/10 (informação do Caio, 10/10). O plano não mudou: furo de 5 mm na tampa, do lado direito, o LED levado por quatro jumpers macho-fêmea e os resistores na protoboard (R63, R64).
- **A data de 27/10.** O documento trata a demonstração de 27/10, com a caixa na parede, como certa (resumo, seção 1, no texto da Figura 1, Quadros 1, 2 e 6, seções 2.3, 3.2 e 7). É data provisória: o professor ainda não publicou o enunciado das entregas seguintes (`docs/entregas.md`).
- **O teste com a caixa fechada.** O Quadro 4 é o teste de 05/10, com o ESP32 solto, e o documento remete o teste completo, com a placa presa, à aula de 06/10 (#33). Esse teste foi feito na aula, com o leitor de volta na caixa, e passou: o cartão foi lido com a tampa fechada (informação do Caio, 10/10). O resultado não está no documento. As respostas "em parte" e "não" do Quadro 4 sobre a firmeza, os fios e a proteção da placa descrevem o ESP32 solto, e não a caixa de hoje, com a placa parafusada (R64).
- **Os vídeos no repositório.** O documento fala dos vídeos da montagem no plural e diz que eles estão no repositório (seção 2.2), e que o vídeo da entrega, de 28 s, está lá com os três vídeos originais (seção 4.1 e Figura 9). O repositório tem um vídeo só, `docs/assets/estrutura/entrega-06-video-2026-10-05.mp4`, de 8,9 s, gravado em 05/10 às 21h19 pela data interna do arquivo: é o teste do cartão com a tampa fechada, na montagem de 05/10 (informação do Caio, 10/10). O paradeiro do vídeo de 28 s e dos três originais é desconhecido.
- **O buzzer.** O documento diz que o buzzer novo apita a cada leitura nos vídeos de 05/10 (seções 2.1 e 7, Quadro 5). A spec registrava a peça como montada e de som fraco, e passou a registrar a troca segundo a Entrega 06; o vídeo curto do buzzer novo na prova de vida, pedido na compra do buzzer (#39), continua pendente.
- **O diário de bordo.** A seção 8 diz que a entrada da semana no `DIARIO.md` registra o modelo no Tinkercad e quem fez cada parte da entrega (Quadro 7). A entrada da semana cita o modelo no Tinkercad só como pedido do professor e nas medidas passadas ao Igor, sem o modelo pronto, e não traz quem fez cada parte da entrega.
- **O leitor fora da caixa.** As Figuras 4 e 10 mostram a caixa sem o leitor, que estava com o João; ele voltou para a caixa na aula de 06/10 (informação do Caio, 10/10).
