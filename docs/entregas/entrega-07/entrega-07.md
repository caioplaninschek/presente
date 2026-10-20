# Entrega 07 — Dashboard

> **Não entregue (14/10, R71, R72).** Na aula de 13/10 o professor disse que o dashboard não tem entrega própria e vai na entrega final de 27/10. Este documento, escrito pelo Igor em 13/10, não foi entregue e fica como base da parte do dashboard na entrega final. O texto abaixo não se reescreve; o Quadro 2 é a situação de 13/10.
>
> *(20/10, R80: o dashboard não tem documento nem postagem próprios, e este texto virou o capítulo do portal do professor, da página de histórico e do dashboard, em [`docs/entregas/entrega-final/relatorio/portal-historico-e-dashboard.md`](../entrega-final/relatorio/portal-historico-e-dashboard.md). É lá que está o estado atual, com os cinco pacotes do firmware na `main` e o que a bateria na placa já conferiu.)*

> **Documento desta entrega, pronto para diagramar.** O texto abaixo substitui o `entrega-07-rascunho.md` e segue a forma da Entrega 06: capa, resumo com palavras-chave, sumário, seções numeradas, figuras e quadros com legenda acima e fonte abaixo.
>
> **Para quem diagrama.** As figuras estão em `docs/assets/dashboard/`, com o nome que cada legenda cita. As capturas de tela são verticais, de celular, e ficam legíveis a cerca de 8 cm de largura, duas por linha; o Quadro 2 é o único elemento largo. O sumário fica sem número de página, que só existe depois da diagramação. Esta entrega não cita obra de terceiros, e por isso não leva a seção de referências que a Entrega 06 tem.
>
> **Fonte dos fatos.** As telas e os números vêm das capturas de 13/10 e do teste informal de 12/10, registrados no `DIARIO.md` e na `docs/spec.md`. O que ainda não rodou está dito como não rodado, na seção 4.

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
Entrega 07 – Dashboard

Professor: Thiago Alberto Ramos Gabriel

Rio de Janeiro RJ
2026

---

## Resumo

Este documento apresenta o dashboard do Presente!, terminal embarcado que registra a presença do aluno em sala de aula pela aproximação do crachá, montado com o microcontrolador ESP32 e o leitor RFID RC522. O dashboard tem duas partes, com públicos e momentos diferentes. A primeira é o portal do professor, uma página que o próprio ESP32 guarda na memória e entrega pela rede Wi-Fi que ele põe no ar: o celular do professor entra nessa rede, a página de login abre sozinha, e é por ela que ele inicia a chamada, acompanha a contagem dos presentes, confere o relatório e envia a chamada ao banco na nuvem. A segunda é a página de histórico, publicada na internet, que lê do banco as aulas já registradas, com os presentes e os faltantes de cada uma, e só entrega os dados depois do login. Como o enunciado desta entrega não foi publicado até 13/10, a equipe seguiu o cronograma de 05/09, que pede um "dashboard (ESP32 obrigatório) ou interface Serial/Processing". O documento mostra as sete telas do portal rodando no navegador com um aparelho simulado, as quatro telas da página de histórico no ar com os dados de teste do banco, e separa, num quadro, o que já foi conferido do que ainda não rodou: o portal servido pela placa tem o teste marcado para a aula de 13/10, e a chamada completa no aparelho depende das duas partes do programa que faltam escrever.

Palavras-chave: dashboard; ESP32; portal cativo; registro de presença; Supabase.

## Sumário

> Sumário sem número de página; a paginação entra na diagramação.

1 O DASHBOARD DO PRESENTE! · 1.1 O portal do professor · 1.2 A página de histórico
2 O PORTAL DO PROFESSOR · 2.1 Login e busca da lista da turma · 2.2 O painel e a chamada · 2.3 Relatório, envio e encerramento · 2.4 As regras que o portal segue
3 A PÁGINA DE HISTÓRICO · 3.1 Login e lista das aulas · 3.2 Os presentes e os faltantes de uma aula · 3.3 A chave do banco e a publicação
4 O QUE JÁ RODA E O QUE FALTA
5 PRÓXIMOS PASSOS
6 ATUALIZAÇÃO DO DIÁRIO DE BORDO

---

## 1 O DASHBOARD DO PRESENTE!

O enunciado do cronograma de 05/09 pede um dashboard com o ESP32 obrigatório, ou uma interface Serial/Processing. O Presente! entrega as duas pontas desse pedido com duas páginas. Uma roda dentro do ESP32 e serve ao professor durante a aula; a outra roda na internet e serve a quem precisa consultar as aulas já registradas. O Quadro 1 resume a divisão.

**Quadro 1** – As duas partes do dashboard.

| Parte | Onde roda | Com quem fala | Quem usa |
|---|---|---|---|
| Portal do professor | No ESP32, pela rede Wi-Fi do aparelho | Com o aparelho | O professor, durante a aula |
| Página de histórico | Na internet, pelo GitHub Pages | Com o banco na nuvem | Quem tem login no banco, depois da aula |

Fonte: elaborado pelos autores (2026).

### 1.1 O portal do professor

O professor não instala aplicativo. Ao chegar à sala, ele conecta o celular na rede Wi-Fi do aparelho, e o ESP32 responde como um portal cativo, o mesmo mecanismo das redes de hotel, de modo que o celular abre sozinho a página de login. Essa página, com as telas que vêm depois dela, é o portal do professor, e é a parte do dashboard que roda no ESP32. Os arquivos do portal ficam gravados na memória da placa, numa área separada da do programa, e o aparelho os entrega pela própria rede, sem internet nenhuma no caminho.

O portal conversa com o aparelho por um conjunto fixo de mensagens, descrito na especificação do projeto: o login, o estado da chamada, o Iniciar, a presença lançada à mão, o relatório, o envio e o encerramento. Para desenvolver as telas sem a placa, a equipe mantém um aparelho simulado, um arquivo que responde a essas mesmas mensagens dentro do navegador. Ele só entra quando o endereço da página pede por ele, com `?mentira=1`; sem isso, o portal fala com o aparelho de verdade.

### 1.2 A página de histórico

Ao fim da aula, o aparelho envia a chamada a um banco de dados na nuvem, na plataforma Supabase, e a página de histórico lê esse banco pela internet. Ela não conversa com o aparelho e não altera nada: as permissões do banco dão a ela o direito de ler, e nenhum outro. O endereço é público, mas a leitura exige e-mail e senha, e sem login o banco não entrega aula nenhuma.

## 2 O PORTAL DO PROFESSOR

As figuras desta seção foram capturadas em 13/10, no navegador do computador, em largura de celular, com o aparelho simulado. Os nomes da turma vêm da lista de teste desse aparelho, e a nota no pé da tela de login, que dá o usuário e a senha de teste, só aparece com ele.

### 2.1 Login e busca da lista da turma

O professor entra com usuário e senha (Figura 1). Aceita a senha, o aparelho busca a lista da turma no banco, pela internet de um hotspot de celular, e enquanto isso o portal mostra a tela "Carregando turma…", com um contador de segundos (Figura 2). Se a busca não terminar em 20 segundos, o aparelho segue sem a lista, no modo offline, e passa a aceitar todo crachá lido; os alunos são conferidos pelo banco no envio.

> Imagem em `docs/assets/dashboard/2026-10-13-portal-1-login.png`.

**Figura 1** – A tela de login do portal.

Fonte: elaborado pelos autores (2026).

> Imagem em `docs/assets/dashboard/2026-10-13-portal-2-carregando-turma.png`.

**Figura 2** – A tela de espera, enquanto o aparelho busca a lista da turma.

Fonte: elaborado pelos autores (2026).

### 2.2 O painel e a chamada

Com a lista carregada, o painel mostra o professor, a disciplina, o modo em que o aparelho está, com a lista ou sem ela, e as seis ações da aula (Figura 3). O Iniciar abre a chamada, e a partir daí o painel conta os presentes e mostra o último crachá lido, com o nome e a hora (Figura 4). O portal pergunta o estado ao aparelho a cada 2 segundos, e é por isso que a contagem acompanha os crachás encostando sem o professor fazer nada.

> Imagem em `docs/assets/dashboard/2026-10-13-portal-3-painel.png`.

**Figura 3** – O painel antes de a chamada começar.

Fonte: elaborado pelos autores (2026).

> Imagem em `docs/assets/dashboard/2026-10-13-portal-4-chamada-aberta.png`.

**Figura 4** – O painel com a chamada aberta e três presentes.

Fonte: elaborado pelos autores (2026).

### 2.3 Relatório, envio e encerramento

O relatório lista os presentes, com a matrícula e a hora da leitura, e os faltantes da turma (Figura 5). O botão "Enviar e fechar a chamada" manda a chamada ao banco; confirmado o envio, o leitor para de registrar, e o painel passa a dizer que a chamada está fechada (Figura 6). Se o professor tocar no botão outra vez, o aparelho responde que a chamada já tinha sido enviada, e nada se duplica. O Encerrar pede confirmação (Figura 7), apaga do aparelho a lista da turma e as presenças da aula e devolve a tela de login, pronta para o próximo professor.

> Imagem em `docs/assets/dashboard/2026-10-13-portal-5-relatorio.png`.

**Figura 5** – O relatório, com os presentes e os faltantes.

Fonte: elaborado pelos autores (2026).

> Imagem em `docs/assets/dashboard/2026-10-13-portal-6-chamada-enviada.png`.

**Figura 6** – O painel depois do envio, com a chamada fechada.

Fonte: elaborado pelos autores (2026).

> Imagem em `docs/assets/dashboard/2026-10-13-portal-7-encerrar.png`.

**Figura 7** – A confirmação do Encerrar.

Fonte: elaborado pelos autores (2026).

### 2.4 As regras que o portal segue

Nenhuma tela avança sem o aparelho confirmar. Todo pedido tem tempo-limite, de 6 segundos, e de 25 no envio, que é o único a sair da sala e ir à internet. Quando o aparelho não responde, ou quando recusa, o portal mostra a falha, libera o botão e deixa a tela onde está, para o professor não ficar com a impressão de que o aparelho obedeceu. O portal manda junto, no login e no Iniciar, a hora do celular, porque o ESP32 não tem relógio com bateria e é essa hora que data a chamada. E, se a página for recarregada com a chamada aberta, o portal pergunta o estado ao aparelho e volta direto ao painel, sem pedir a senha de novo. Essas regras foram conferidas no navegador, com o aparelho simulado; no aparelho de verdade, elas ainda não foram testadas.

## 3 A PÁGINA DE HISTÓRICO

As figuras desta seção foram capturadas em 13/10, na página no ar, com um login da equipe e os dados de teste do banco: seis aulas inventadas da turma `4172CMPN6A_P1` e quatro deixadas pelas verificações automáticas do banco, da turma `TESTE01`.

### 3.1 Login e lista das aulas

A página está publicada em https://caioplaninschek.github.io/presente/ e abre pedindo e-mail e senha (Figura 8).

> Imagem em `docs/assets/dashboard/2026-10-13-historico-1-login.png`.

**Figura 8** – A tela de login da página de histórico.

Fonte: elaborado pelos autores (2026).

A lista mostra as aulas da mais recente para a mais antiga, cada uma com a data e a hora, o professor, a turma, a disciplina, a situação e o modo (Figura 9). A situação diz se a aula foi finalizada ou cancelada, e o modo diz se o aparelho tinha a lista da turma, caso em que a aula aparece como "Com internet", ou não a tinha, caso em que aparece como "Sem internet". A página distingue também a aula ainda aberta, mas nenhuma aula do banco está nessa situação, e por isso ela não aparece na figura.

> Imagem em `docs/assets/dashboard/2026-10-13-historico-2-lista.png`.

**Figura 9** – A lista das aulas registradas no banco.

Fonte: elaborado pelos autores (2026).

### 3.2 Os presentes e os faltantes de uma aula

Ao abrir uma aula, a página mostra os presentes, cada aluno uma vez só, com a matrícula e o horário da primeira leitura, e os faltantes da turma. A presença lançada à mão pelo professor vem marcada, com o motivo e com quem a lançou. As Figuras 10 e 11 são o começo e o fim da aula de teste de 18/08, registrada sem internet: 34 presentes, um deles lançado à mão porque o aluno esqueceu o crachá, e 6 faltantes.

> Imagem em `docs/assets/dashboard/2026-10-13-historico-3-aula-topo.png`.

**Figura 10** – O começo da aula de 18/08, com os presentes.

Fonte: elaborado pelos autores (2026).

> Imagem em `docs/assets/dashboard/2026-10-13-historico-4-aula-fim.png`.

**Figura 11** – O fim da mesma aula, com a presença lançada à mão e os faltantes.

Fonte: elaborado pelos autores (2026).

### 3.3 A chave do banco e a publicação

Nenhuma credencial do banco fica no repositório. A cada publicação, o GitHub monta o arquivo de configuração da página com o endereço do banco e a chave publicável dele, guardados como segredos do repositório, e só então põe a página no ar. Essa chave fica visível para quem abre a página, e a equipe assumiu esse custo com uma compensação: sozinha, ela não lê nada, porque o banco só libera a leitura das tabelas depois do login.

## 4 O QUE JÁ RODA E O QUE FALTA

A placa que está na caixa tem gravadas as duas primeiras das cinco partes do programa único do aparelho: o portal dentro do ESP32, com a rede própria e o login, que é a parte 1, e a busca da lista da turma pela internet, que é a parte 2. A parte 2 foi testada de modo informal em 12/10, pelo monitor serial, antes de o portal estar ligado ao aparelho de verdade. O Quadro 2 separa o que foi conferido do que ainda não rodou.

**Quadro 2** – O que já roda e o que falta, em 13/10.

| O quê | Situação | Como foi conferido |
|---|---|---|
| Portal no navegador, com o aparelho simulado, do login ao Encerrar | Funciona | No navegador do computador, em 13/10 (Figuras 1 a 7) |
| Página de histórico no ar, com login e as aulas de teste do banco | Funciona, com os dados de teste | Na página no ar, em 13/10, com um login da equipe (Figuras 8 a 11) |
| Busca da lista da turma pelo ESP32, a parte 2 | Funciona em teste informal | Em 12/10, pelo monitor serial: 40 alunos em cerca de 8 s; sem hotspot, modo offline em cerca de 7 s |
| Portal servido pelo ESP32, falando com o aparelho de verdade | Não testado | O teste está marcado para a aula de 13/10 |
| Iniciar, leitura do crachá na chamada, relatório e envio ao banco pelo aparelho | Não escrito | Dependem das partes 3 e 4 do programa único |
| Teste oficial das partes 1 e 2 na placa | Não feito | Espera a revisão do código por um integrante que não o escreveu |

Fonte: elaborado pelos autores (2026).

Com as partes 1 e 2 na placa, o portal de verdade deve mostrar a tela de login sem a nota de teste, recusar a senha errada, passar pela espera da turma e chegar ao painel. O Iniciar, o envio e o Encerrar ainda são recusados pelo aparelho, com aviso na tela, porque as mensagens deles chegam nas partes 3 e 4. Esse é o comportamento previsto pelo código; o que o aparelho de fato mostra, só o teste da aula vai dizer.

## 5 PRÓXIMOS PASSOS

A parte 3 do programa único traz a chamada: o Iniciar, a leitura do crachá contra a lista da turma, com o LED e o buzzer dizendo ao aluno se a presença foi registrada, e a presença lançada à mão. A parte 4 traz o relatório, o envio ao banco e o encerramento. Com as duas, o painel da Figura 4 passa a contar crachás de verdade, e a aula enviada passa a aparecer na página de histórico, fechando o caminho que vai do crachá encostado à consulta na internet. Antes de cada parte ir oficialmente para a placa, um integrante que não a escreveu revisa o código. Essas etapas estão previstas para a entrega do protótipo completo.

## 6 ATUALIZAÇÃO DO DIÁRIO DE BORDO

O diário de bordo fica no arquivo `DIARIO.md` do repositório público do projeto e é atualizado a cada semana. A entrada da semana 7 registra a semana por frente: os crachás do projeto, as partes 1 e 2 do programa único, a ligação do portal ao aparelho de verdade, a página de histórico e a decisão de entregar o dashboard pela leitura do cronograma de 05/09. O Quadro 3 registra quem fez cada parte desta entrega.

**Quadro 3** – Quem fez cada parte desta entrega.

| Integrante | Responsável por |
|---|---|
| Caio | As partes 1 e 2 do programa único do aparelho, o teste informal da parte 2, o banco na nuvem e os dados de teste, as capturas de tela e o rascunho deste documento |
| Gabriel | A diagramação deste documento |
| Igor | A ligação do portal ao aparelho de verdade (#37), a página de histórico e a publicação dela (#38) e a redação deste documento |

Fonte: elaborado pelos autores (2026).

Repositório: https://github.com/caioplaninschek/presente

Página de histórico: https://caioplaninschek.github.io/presente/
