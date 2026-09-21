# Entrega 05: Protótipo eletrônico com testes documentados

> **Espaço de montagem compartilhado**, no mesmo papel que o `entrega-04-rascunho.md` teve na entrega anterior. Cada seção traz de onde sai o conteúdo, quem escreve e até quando. A formatação em ABNT, a capa e a numeração são da montagem final (#30), que produz a transcrição em `entrega-05.md` ao lado do PDF.
>
> **Contrato do artefato.** Finalidade: mostrar ao professor o protótipo eletrônico funcionando e os testes que o provam. Público: o professor que avalia, não desenvolvedor. Registro: acadêmico direto, primeira pessoa do plural. **Diferença em relação à Entrega 04:** o enunciado pede o código-fonte com as alterações identificadas, então a seção 2 **nomeia arquivos e pastas**. Nas demais seções a regra anterior continua valendo. Formato: PDF montado sobre a cópia do `.docx` da Entrega 04, da qual herda capa, margens, estilos e numeração. **Não há limite de páginas publicado para esta entrega** (conferido no `entregas.md` em 20/09); o alvo é o formato das anteriores, sem teto rígido. Fonte dos fatos: `spec.md`, o repositório e o `DIARIO.md`; fato que não está lá se pergunta, não se preenche.
>
> **Modalidade protegida, mais apertada que na Entrega 04.** Esta entrega é sobre o que funcionou de verdade. Nenhum verbo pode apresentar como testado o que não foi executado na placa. Teste que não rodou entra escrito como não rodou, com o motivo: o enunciado pede o resultado obtido, e "não obtido" é um resultado declarável. O que reprova é a omissão.
>
> ⚠️ **Este bloco de trabalho sai do documento na montagem.**
>
> ⚠️ **Duas divergências de hardware em aberto em 20/09, que podem obrigar a corrigir texto já escrito:**
> 1. **O LED pode não ser RGB.** O João descreve a peça como "um verde, um amarelo, um vermelho"; o §4.1 da spec e os quatro programas assumem vermelho, verde e azul nos GPIO 25, 26 e 27. As seções 1 e 2 abaixo seguem a spec. Se a peça for outra, as duas seções mudam.
> 2. **A ligação do buzzer não tem veredito** (#23). A spec manda checar antes de afirmar, e nenhuma frase deste rascunho afirma transistor.

---

## 1 DESCRIÇÃO DA INTEGRAÇÃO

*Origem: §3, §4 e §5 da spec, mais as imagens `diagrama-blocos.png` (Entrega 03) e `fluxograma-logica.png` (Entrega 04) · Dono: Caio (#30) · Prazo: domingo, 20/09 · **escrita***

O sistema registra a presença do aluno em sala pelo crachá, sem chamada oral e sem lista em papel. O protótipo se organiza em três blocos, na ordem em que a informação atravessa o aparelho.

**O sensor é o leitor RFID/NFC RC522**, de 13,56 MHz. Ele não lê nome nem matrícula: lê o **UID** da tag, o número único que cada crachá carrega de fábrica, com 4 bytes nas tags Mifare e 7 bytes nas NTAG215. A comunicação com o microcontrolador é feita por **SPI**, no barramento VSPI por hardware, com o sinal de seleção no GPIO 5, o relógio no 18, os dados de ida no 23, os de volta no 19 e o reinício no 22. O módulo trabalha estritamente em 3,3 V, tensão que o regulador da própria placa fornece.

**O processamento é o ESP32 DevKit V1.** Ele recebe o UID e decide sem consultar rede nenhuma: se aquele crachá ainda não foi registrado nesta sessão, o evento é gravado; se o mesmo crachá voltar ao campo em menos de cinco segundos, nada acontece, porque é a mesma aproximação sendo lida de novo; e se ele já estava registrado, o aparelho avisa que reconheceu, mas não grava uma segunda vez. A decisão se apoia numa lista gravada no sistema de arquivos interno do próprio microcontrolador, o LittleFS, relida toda vez que o aparelho liga. É o que impede que uma queda de energia no meio da aula apague a chamada.

**Os atuadores são o LED RGB**, ligado aos GPIO 25, 26 e 27, **e o buzzer**, no GPIO 33. Eles carregam a resposta que o aluno vê e ouve, e as três respostas são deliberadamente diferentes entre si: duas piscadas verdes com um bip de 100 ms quando a presença entra; uma piscada verde, sem som, quando o crachá já estava registrado; e nada, nem luz nem som, na releitura dentro da janela de cinco segundos. O aluno não precisa olhar para tela nenhuma para saber o que aconteceu.

O ciclo completo, portanto, é **crachá → RC522 → ESP32 → decisão → LED e buzzer → registro em disco**, e se fecha inteiro dentro do aparelho, sem internet. A rede serve a dois momentos que não são esse: o professor abre a sessão pelo celular, numa página servida pelo próprio ESP32, e o relatório sobe para a nuvem no fim da aula.

> Imagem do diagrama de blocos.

Figura 1 – Diagrama de blocos do sistema.

> Imagem do fluxograma, a 16 cm de largura.

Figura 2 – Fluxograma da lógica do programa.

### 1.1 Ligações do ESP32

| Componente | Pino do módulo | GPIO | Direção |
|---|---|---|---|
| RC522 (sensor) | SDA / SS | 5 | Saída (SPI CS) |
| RC522 | SCK | 18 | Saída |
| RC522 | MOSI | 23 | Saída |
| RC522 | MISO | 19 | Entrada |
| RC522 | RST | 22 | Saída |
| LED RGB (atuador 1) | R | 25 | Saída |
| LED RGB | G | 26 | Saída |
| LED RGB | B | 27 | Saída |
| Buzzer ativo (atuador 2) | sinal | 33 | Saída |

Uma fonte de 5 V na tomada alimenta o conjunto, porque o aparelho é fixo e foi pensado para ficar preso à parede da sala; o leitor recebe os 3,3 V do regulador da própria placa. Cada linha do LED tem um resistor de 220 Ω em série. O projeto usa **um sensor só**: o professor dispensou esta equipe da exigência de dois.

---

## 2 CÓDIGO-FONTE ATUALIZADO

*Origem: árvore da `main` e o histórico desde o commit da Entrega 04 · Dono: Caio (#30) · Prazo: domingo, 20/09 · **escrita***

O código está no repositório público do projeto, em `github.com/caioplaninschek/presente`. A Entrega 04 foi entregue sem nenhuma linha de firmware escrita para as peças; desde então o repositório recebeu **seis programas, três módulos compartilhados, o portal do professor e o banco na nuvem**.

A decisão de organização mais importante do período foi escrever **um programa por experimento**. Em vez de um firmware único e grande, cada coisa que precisa ser provada na placa virou um programa próprio, na sua pasta em `src/`, com o seu ambiente de compilação declarado no `platformio.ini`. A razão é que há um microcontrolador só para cinco pessoas: com os programas separados, cada frente escreve o seu sem esbarrar na do outro, e quem está com a placa grava um de cada vez, sabendo exatamente o que aquele teste prova e o que ele não prova.

| Pasta em `src/` | O que o programa faz | Testes que ele atende |
|---|---|---|
| `ambiente/` | Pisca o LED da placa e escreve uma frase por segundo no monitor serial. Prova que o computador compila e grava | Nenhum. É pré-requisito de todos |
| `prova-de-vida/` | Percorre vermelho, verde e azul, um segundo cada, e toca o bip. O nome da cor acesa sai no monitor serial | Atuadores isolados |
| `leitor/` | Encosta a tag, o UID aparece no monitor serial em hexadecimal maiúsculo. Sem LED, sem som, sem gravar | Sensor isolado |
| `encostou/` | Junta os dois: encostou, piscou verde, apitou, e não registra duas vezes | **Testes 1 e 2** |
| `persistencia/` | O mesmo, com o arquivo de eventos atrás: a lista de quem já está registrado sobrevive ao reinício | **Teste 8** |
| `radio/` | Mede a memória livre no instante da conexão segura e observa o comportamento do painel no pulo de canal | **Testes 7c e 7d** |

Para que esses programas não repetissem o mesmo código, o que é comum a eles vive em três módulos, na pasta `lib/`. O módulo `rfid/` faz a leitura do UID de 4 e de 7 bytes e controla a janela de silêncio de cinco segundos; o `feedback/` comanda o LED e o buzzer por máquina de estados, sem nunca travar o programa numa espera; e o `storage/` grava os eventos no LittleFS, uma linha por evento, em modo de acréscimo, relendo o arquivo quando o aparelho liga.

As versões das bibliotecas estão fixadas de propósito no `platformio.ini`: a plataforma `espressif32` na 7.1.3, a `MFRC522` na 1.4.12 e a `ArduinoJson` na 6.21.6. O firmware precisa compilar igual nas cinco máquinas da equipe, e mudar uma versão é decisão de grupo, não de quem estiver editando no momento. O sistema de arquivos também está declarado explicitamente como LittleFS, em lugar do padrão da plataforma, para que a gravação do sistema de arquivos não apague o portal já gravado na mesma partição.

Quatro regras de escrita valem para todos os programas e vêm da especificação: nenhuma espera bloqueante no laço principal, com todo tempo contado por marcos de relógio; SPI por hardware; documento JSON de tamanho fixo, sem concatenar texto em memória; e log no monitor serial com níveis, porque é dele que saem os números dos testes.

Fora do firmware, o mesmo período produziu as três telas do professor (login, painel e relatório), que rodam no navegador contra dados de mentira, e o banco na nuvem no Supabase, com o esquema, os dados de teste e um roteiro de verificação. Nenhum dos dois está dentro do aparelho ainda; os dois pertencem à etapa de integração.

A especificação do projeto acompanhou o código. Ela passou por quatro rodadas de decisão entre 13 e 16 de setembro, e três delas alcançam diretamente este protótipo: uma presença por crachá por sessão, decidida pela lista em disco e não pela memória; os 200 ms do teste 1 contados do início da conversa com o leitor, e declarados como piso; e o formato do que o aparelho grava em disco, que passou a ser uma linha por evento, com o tempo medido desde o instante em que a placa ligou.

---

## 3 TESTES DOCUMENTADOS

*Origem: `docs/testes-22-09.md` (#28, Gabriel) · Dono: Gabriel, montagem do Caio (#30) · Prazo: segunda, 21/09 · **formulário montado em 20/09, resultados em branco***

> ⚠️ **Estado em 20/09, 12h53:** nenhum teste rodou. O formulário abaixo traz a descrição e o resultado esperado de cada teste, que saem da especificação e não dependem de execução. **O resultado obtido, os problemas, as correções e a evidência são preenchidos com o retorno do João.** Nada aqui pode ser dado como feito antes de a evidência existir.

Os testes são executados na placa real, com o aparelho montado, e verificam apenas o comportamento externo: o que o LED e o buzzer fazem, o que sai no monitor serial e o que fica gravado em disco. Cada teste deixa evidência gravada, que pode ser uma foto, um vídeo curto ou o trecho do log com o horário, e ela fica versionada no repositório.

> Fotografia do protótipo montado, de cima, com as ligações visíveis.

Figura 3 – Protótipo montado. *(⚠️ marca de trabalho, sai na montagem: a foto é pendência da #22.)*

### Teste 1: Crachá de 4 bytes, três aproximações seguidas

- **Descrição:** encostar uma tag Mifare, de UID com 4 bytes, três vezes em sequência.
- **Resultado esperado:** um único registro, e não três, com a resposta do LED e do buzzer em menos de 200 ms, contados do início da conversa com o leitor. O número é declarado como piso, e não como medida exata: o tempo em que a tag espera no campo até o programa ir buscá-la não é visível de dentro do aparelho.
- **Resultado obtido:** `[a preencher]`
- **Problemas e correções:** `[a preencher]`
- **Evidência:** `[a preencher: vídeo e trecho do log]`

### Teste 2: Crachá de 7 bytes

- **Descrição:** encostar uma tag NTAG215, de UID com 7 bytes.
- **Resultado esperado:** o registro acontece pelo UID, e nada é escrito na tag.
- **Resultado obtido:** `[a preencher]`
- **Problemas e correções:** `[a preencher]`
- **Evidência:** `[a preencher: lista dos UIDs lidos, em texto]`

### Teste 8: Estresse: 30 toques com um reinício no meio

- **Descrição:** trinta aproximações seguidas, com o aparelho desligado e religado no meio da sequência.
- **Resultado esperado:** nenhuma duplicata, ou seja, um registro por crachá, com quem já estava registrado continuando recusado depois do reinício; e nenhuma perda do que já havia sido gravado.
- **Resultado obtido:** `[a preencher]`
- **Problemas e correções:** `[a preencher]`
- **Evidência:** `[a preencher: conteúdo do arquivo de eventos e a contagem de boot]`

### Teste 7d: Memória livre no instante da conexão segura

- **Descrição:** com a rede do aparelho, o serviço de nomes, o servidor de páginas e a lista da turma de pé, medir a memória livre no instante em que a conexão segura com a nuvem é aberta.
- **Resultado esperado:** pelo menos 40 KB livres. Abaixo disso, o arranjo simultâneo reprova. Medido sem nenhum celular conectado ao aparelho o número sai otimista, e um valor entre 40 e 45 KB é suspeito, não aprovado.
- **Resultado obtido:** `[a preencher: o menor número das três rodadas]`
- **Problemas e correções:** `[a preencher]`
- **Evidência:** `[a preencher: trecho do log das três rodadas]`

### Teste 7c: O painel sobrevive ao pulo de canal

- **Descrição:** com o professor logado no painel pelo celular, o aparelho conecta a um hotspot que está em outro canal.
- **Resultado esperado:** o painel continua aberto, sem pedir login de novo; se cair, reconecta sozinho em menos de cinco segundos. Com dois celulares do mesmo sistema operacional, o veredito sai parcial.
- **Resultado obtido:** `[a preencher]`
- **Problemas e correções:** `[a preencher]`
- **Evidência:** `[a preencher: trecho do log e o que foi observado nos dois celulares]`

---

## 4 O QUE NÃO ENTRA NESTA ENTREGA, E POR QUÊ

*Origem: §8 da spec (lacunas conhecidas), corpo das #7 e #28 · Dono: Caio (#30) · Prazo: domingo, 20/09 · **escrita***

O plano de testes da especificação tem onze itens, e esta entrega apresenta cinco. A ausência dos outros é escolha declarada, não descuido, e cada grupo tem um motivo próprio.

**Os testes 3, 4, 5 e 6** (crachá fora da turma, modo offline, reenvio do relatório e dois professores em sequência) dependem de a lista da turma estar dentro do aparelho, e ela só passa a existir quando o ESP32 falar com a nuvem, na etapa de integração prevista entre o fim de setembro e o começo de outubro. Sem essa lista, o aparelho não tem como saber que um crachá não pertence à turma, e os quatro testes não teriam o que verificar.

**Os testes 7 e 7b** só se tornam obrigatórios se a medição do rádio indicar o arranjo alternado. Eles verificam a alternância entre a rede do aparelho e a rede externa, que deixa de existir caso o arranjo simultâneo seja aprovado. A decisão sai dos testes 7c e 7d desta entrega.

**A máquina de estados completa** do firmware integrado, com os estados de rede e de sessão, e a retomada da sessão depois de um reinício também ficam para a integração, porque dependem do portal rodando dentro do aparelho, e o portal ainda roda no navegador.
