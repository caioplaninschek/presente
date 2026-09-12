# Entrega 04 — Especificação e preparação para o desenvolvimento

> **Espaço de montagem compartilhado**, no mesmo papel que o `entrega-03-rascunho.html` teve na entrega anterior. Cada seção traz de onde sai o conteúdo, quem escreve e até quando. A formatação em ABNT, a capa e a numeração são da montagem final (#13), que produz a transcrição em `entrega-04.md` ao lado do PDF.
>
> **Contrato do artefato.** Finalidade: mostrar ao professor o que o projeto especificou e preparou para o desenvolvimento. Público: o professor que avalia, não desenvolvedor. Registro: acadêmico direto, primeira pessoa do plural, sem nome de arquivo, de função nem de commit. Formato: PDF de 2 a 4 páginas — **assumindo que a capa conta**, sobram três páginas para sete seções, cinco delas ocupadas por diagrama, tabela, fluxograma, pseudocódigo e tabela; a seção 1 tem meia página, de 250 a 350 palavras. Fonte dos fatos: `spec.md` e `DIARIO.md`; fato que não está lá se pergunta, não se preenche.
>
> **Modalidade protegida.** Os Planos A e B de rádio seguem **em aberto**. Existem critérios de medição, não veredito, e nenhuma medição de bancada foi feita — nenhum verbo do documento pode apresentar como escolhido ou medido o que ainda é plano.
>
> ⚠️ **Este bloco de trabalho sai do documento na montagem.**

## 1 EVOLUÇÃO DESDE A ENTREGA ANTERIOR

*Origem: §2 da spec (rodadas 7 e 8), §9, §10 e a semana 4 do diário · Dono: Caio (#11) · Prazo: domingo, 13/09 · **escrita***

O obstáculo mais caro do período é físico. Há um microcontrolador só para cinco pessoas, e a divisão de responsabilidades de 07/09 descrevia cinco frentes com um dono cada, várias delas precisando da placa ao mesmo tempo. Em vez de o equipamento circular, ele ficou com o João, e são as tarefas que chegam até a placa prontas para executar, com o programa escrito e o passo a passo do que deve aparecer. Uma mudança em relação à Entrega 03: a tabela de responsabilidades descreve os papéis, e o que distribui o trabalho hoje são tarefas com dono e prazo.

Passamos a decidir por escrito, em tópicos abertos no repositório, e a primeira rodada decidida assim corrigiu um erro da nossa própria especificação. O João contestou a afirmação de que o microcontrolador não pode ser rede e cliente de internet ao mesmo tempo, e a documentação do fabricante confirma que o modo simultâneo é nativo. A decisão registrada em 05/09, de alternar entre os dois modos, partia de premissa errada e foi reaberta.

Com isso o principal desafio técnico mudou de forma. A pergunta deixou de ser como alternar entre os modos sem derrubar o professor e passou a ser qual dos dois arranjos o hardware sustenta, porque o impedimento que resta é de memória: a conexão segura com a nuvem exige de 40 a 50 KB livres no instante em que é aberta, e esse espaço precisa sobrar com a rede do aparelho, o servidor de páginas e a lista da turma de pé. Os dois entraram no plano de testes com critérios de aprovação, e medi-los na bancada é o primeiro código do projeto.

No mesmo período o modelo de dados ganhou três entidades propostas pelo Cauã — o professor com cadastro próprio, a turma como tabela e a aula com estado, o mecanismo que impede o envio duplicado do relatório —, o Igor assumiu a tela de histórico, o João comprou todas as peças e a especificação recebeu as histórias de usuário, as ligações pino a pino, o pseudocódigo e a política de teste.

## 2 ARQUITETURA DO SISTEMA

*Origem: §3 da spec, imagem exportada de `diagrama-blocos.drawio` · Dono: Caio (#13) · Prazo: segunda, 14/09 · **pendente***

⚠️ O diagrama tem hoje o rótulo em negrito `alterna — nunca os dois ao mesmo tempo`, derrubado pelo R19. Trocar a string no XML e reexportar o PNG antes de inserir, senão o diagrama contradiz a seção 1 dentro do mesmo documento.

## 3 LIGAÇÕES DO ESP32

*Origem: §4.1 da spec, tabela `componente | GPIO | direção` · Dono: Caio (#13) · Prazo: segunda, 14/09 · **pendente***

⚠️ O modelo do enunciado pressupõe dois sensores. A dispensa (R18) provavelmente precisa ser repetida em uma linha, como a Entrega 03 fez no §4.

## 4 LÓGICA DO PROGRAMA

*Origem: fluxograma novo, em `fluxograma-logica.drawio` (R27) · Dono: Gabriel (#10) · Prazo: domingo, 13/09 · **pendente***

Fluxograma, sem código.

## 5 PSEUDOCÓDIGO

*Origem: §5 da spec · Dono: Caio (#13) · Prazo: segunda, 14/09 · **pendente***

## 6 PLANO DE TESTES

*Origem: §8 da spec, com as lacunas conhecidas — os testes 7 e 7b, o arquivo de sessão e a máquina de estados completa · Dono: Caio (#13) · Prazo: segunda, 14/09 · **pendente***

⚠️ A observação sobre os testes 3, 4, 5 e 6 não entra aqui: ela foi escrita na seção 7.

## 7 PREPARAÇÃO PARA 21/09

*Origem: checklist novo, a partir da divisão em tarefas de 11/09 · Dono: Caio (#12) · Prazo: domingo, 13/09 · **escrita***

O ESP32, o leitor de crachá, o LED e o buzzer estão comprados e em mãos; as ligações estão na seção 3 e o fluxograma da lógica, na seção 4. O que falta — código inicial, componentes testados individualmente e divisão das tarefas definida — está na tabela abaixo. Esta entrega e o protótipo eletrônico da Entrega 05 são os dois marcos do período, e por isso algumas datas chegam a 22/09.

| Integrante | O que entrega até 21-22/09 |
|---|---|
| Gabriel | O fluxograma da lógica (13/09) e o dossiê de evidências dos testes (22/09) |
| Cauã | Os programas do leitor de crachá (19/09), do verde com bip sem registro duplicado (21/09) e da gravação que sobrevive ao reinício (22/09) |
| Igor | As três telas do professor no navegador: login, painel de seis botões e relatório (21/09) |
| Caio | O ambiente de compilação da placa (14/09); o banco na nuvem, o programa do teste de rádio e o roteiro de bancada dele (17/09); e a decisão de qual arranjo de rádio vale (21/09) |
| João | A montagem da protoboard e a checagem do buzzer (16/09), a prova de vida do LED e do bip (17/09), a medição da memória livre na conexão segura (19/09) e a verificação do painel na troca de canal (20/09); e roda na placa, de 14 a 22/09, o que os outros preparam, gravando a evidência de cada teste |

Duas coisas travam o resto. Nada roda na placa antes de o ambiente de compilação estar de pé na máquina que a tem, em 14/09. E a verificação do painel é o único teste que exige dois celulares junto da placa, o que depende de um encontro combinado.

Os testes 3, 4, 5 e 6 do plano de testes não ficam prontos em 22/09: dependem de a lista da turma estar dentro do aparelho, o que só acontece quando ele passar a falar com a nuvem, entre o fim de setembro e o começo de outubro.
