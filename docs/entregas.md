# Entregas — enunciados do professor

Fonte: Teams da disciplina, aba Tarefas. **O prazo de cada entrega é o do enunciado do professor.** As datas marcadas com \* ainda não têm enunciado publicado: vêm do cronograma passado ao grupo em 05/09 e se conferem quando o enunciado sair. Os dois enunciados já publicados, das Entregas 03 e 04, citam **21/09** como a data da preparação. ~~O cronograma válido é o revisado (datas de terça); as datas de segunda são da outra turma~~ — informação errada, corrigida em 13/09.

| Data | Entrega | Estado |
|---|---|---|
| 27/08 | Formação da equipe, tema e problema | ✅ entregue |
| 02/09 | Canvas + lista de componentes + diagrama em blocos | ✅ entregue |
| 07/09 20:00 | Projeto detalhado da solução | ✅ entregue |
| 14/09 | Projeto de desenvolvimento: programação planejada, lógica, conexões, plano de testes | ✅ entregue |
| 22/09\* | Protótipo eletrônico: ESP32 + sensores + atuador + testes documentados | ⬜ |
| 29/09\* | Estrutura física do protótipo | ⬜ |
| 05/10\* | Dashboard (ESP32 obrigatório) ou interface Serial/Processing | ⬜ |
| 20/10\* | Protótipo completo + vídeo de demonstração (2 minutos) | ⬜ |
| 27/10\* | Exposição oficial na Semana da Computação | ⬜ |
| 30/11\* | Relatório técnico completo + versão final aprimorada | ⬜ |

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
