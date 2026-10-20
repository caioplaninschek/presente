> **Capítulo do relatório técnico: o portal do professor, a página de histórico e o dashboard (entrega do dashboard, #52).** Quem escreve: Igor. O que entra: a Entrega 07 atualizada para o estado final, com os pacotes 3 a 5 do firmware na `main` e os resultados da bateria na placa, transformada em capítulo deste relatório. Fontes: a Entrega 07 (`docs/entregas/entrega-07/`), a R55 e a R80 da `docs/spec.md`, e as rotas entre o portal e o aparelho no §6.1. Extensão: em branco até o professor responder sobre o número de páginas (ver o `indice.md`); hoje o texto tem dois quadros e onze figuras. A numeração da seção e das subseções fica com a consolidação (#71). Ficam de fora, por serem de outras seções: o que o firmware faz por dentro (#67) e as medições e a tabela dos testes (#69), que o capítulo pode citar. Este bloco sai na consolidação.

# O PORTAL DO PROFESSOR, A PÁGINA DE HISTÓRICO E O DASHBOARD

O projeto integrador pede um dashboard, com o ESP32 obrigatório, ou uma interface Serial/Processing. Entregamos as duas pontas desse pedido em duas páginas, com públicos e momentos diferentes. A primeira é o portal do professor, que o próprio ESP32 guarda na memória e serve pela rede Wi-Fi que ele põe no ar: é por ela que o professor inicia a chamada, acompanha a contagem dos presentes, confere o relatório e manda a chamada ao banco. A segunda é a página de histórico, publicada na internet, que lê do banco as aulas já registradas e mostra os presentes e os faltantes de cada uma. O Quadro 1 resume a divisão.

**Quadro 1: As duas partes do dashboard**

| Parte | Onde roda | Com quem fala | Quem usa |
|---|---|---|---|
| Portal do professor | No ESP32, pela rede Wi-Fi do aparelho | Com o aparelho | O professor, durante a aula |
| Página de histórico | Na internet, pelo GitHub Pages | Com o banco na nuvem | Quem tem login no banco, depois da aula |

Fonte: elaborado pelos autores (2026).

Descrevemos aqui o que as duas páginas fazem e por que as fizemos assim. A verificação na placa, com os números de cada rodada, é assunto da seção dos testes.

## O portal do professor

O professor não instala aplicativo. Ao chegar à sala, ele conecta o celular na rede Wi-Fi do aparelho, e o ESP32 responde como um portal cativo, o mesmo mecanismo das redes de hotel, de modo que o celular abre sozinho a página de login. Os arquivos do portal moram na pasta `data/` do repositório, que o PlatformIO transforma na imagem de memória gravada na placa, numa área separada da do programa. São uma página só, sem biblioteca e sem nada vindo da internet, porque na sala a rede do aparelho não tem internet para buscar coisa alguma.

### O contrato com o aparelho

O portal e o firmware conversam por um conjunto fixo de mensagens, que fechamos antes de escrever os dois lados, para que cada um pudesse ser escrito e conferido sozinho. O Quadro 2 lista as rotas.

**Quadro 2: As mensagens entre o portal e o aparelho**

| Rota | Para quê |
|---|---|
| `POST /api/login` | Entrar com usuário e senha, e mandar junto a hora do celular |
| `GET /api/estado` | Perguntar em que pé está a chamada, com as contagens e o último crachá lido |
| `GET /api/presencas` | Pedir as listas de presentes e de faltantes, que a tela do relatório desenha |
| `POST /api/seguir-offline` | Desistir da espera pela lista da turma e seguir sem ela |
| `POST /api/iniciar` | Abrir a chamada, com a hora do celular |
| `POST /api/manual` | Lançar à mão a presença de quem esqueceu o crachá |
| `GET /api/relatorio` | Baixar o relatório da chamada no celular do professor |
| `POST /api/enviar` | Mandar a chamada ao banco e fechar a chamada |
| `POST /api/encerrar` | Apagar do aparelho a aula e voltar à tela de login |

Fonte: elaborado pelos autores (2026).

Três regras valem para todas elas. Os dois lados falam em JSON, e todo erro vem com um código, pelo qual o portal escolhe a mensagem que mostra ao professor. Toda rota, menos o login e o estado, exige o cookie de sessão; sem ele, o aparelho responde que não há login, e o portal volta à tela de login, onde quer que o professor estivesse. E todo pedido tem tempo-limite, de 6 segundos, menos o envio ao banco, que espera 25, por ser o único que sai da sala e vai à internet.

A hora merece uma linha à parte. O ESP32 não tem relógio com bateria, e é a hora do celular do professor, mandada no login e no Iniciar, que data a chamada. O portal a converte para o horário de Brasília antes de mandar, de modo que o aparelho recebe sempre o mesmo formato, venha o celular do fuso que vier.

Para escrever as telas sem a placa, mantemos um aparelho de mentira, um arquivo de JavaScript que responde a essas mesmas mensagens dentro do navegador. Ele só entra quando o endereço da página pede por ele, com `?mentira=1`; sem isso, o portal fala com o aparelho de verdade. É dele que vêm as figuras deste capítulo, porque com ele mostramos as telas cheias, com a turma inteira, sem depender de uma aula com quarenta alunos.

### Do login ao painel

O professor entra com usuário e senha (Figura 1). Aceita a senha, o aparelho busca a lista da turma no banco, pela internet de um hotspot de celular, e enquanto isso o portal mostra a tela de espera, com um contador de segundos (Figura 2). Se a busca não terminar em 20 segundos, o aparelho segue sem a lista, no modo offline, e passa a aceitar todo crachá lido; quem é da turma, o banco confere no envio. O professor também pode desistir da espera e pedir o modo offline, se já souber que naquela sala não vai haver internet.

**Figura 1: A tela de login do portal**

![A tela de login do portal](../../../assets/dashboard/2026-10-13-portal-1-login.png)

Fonte: elaborado pelos autores (2026).

**Figura 2: A tela de espera, enquanto o aparelho busca a lista da turma**

![A tela de espera, enquanto o aparelho busca a lista da turma](../../../assets/dashboard/2026-10-13-portal-2-carregando-turma.png)

Fonte: elaborado pelos autores (2026).

Com a busca terminada, o painel mostra o professor, a disciplina, o modo em que o aparelho está, com a lista ou sem ela, e as seis ações da aula (Figura 3). O portal pergunta o estado ao aparelho a cada 2 segundos, e é por isso que a contagem acompanha os crachás encostando sem o professor fazer nada.

**Figura 3: O painel antes de a chamada começar**

![O painel antes de a chamada começar](../../../assets/dashboard/2026-10-13-portal-3-painel.png)

Fonte: elaborado pelos autores (2026).

### A chamada

O Iniciar abre a chamada, e a partir daí o painel conta os presentes e mostra o último crachá lido, com o nome e a hora (Figura 4). O aparelho recusa o Iniciar enquanto a busca da turma não termina, para a aula não começar meio online e meio offline, e recusa a hora que não é uma hora válida, como a de um celular com o relógio zerado.

**Figura 4: O painel com a chamada aberta e três presentes**

![O painel com a chamada aberta e três presentes](../../../assets/dashboard/2026-10-13-portal-4-chamada-aberta.png)

Fonte: elaborado pelos autores (2026).

O aluno que esqueceu o crachá entra pelo lançamento manual, em que o professor digita a matrícula e, se quiser, o motivo. Com a lista da turma, o aparelho recusa a matrícula que não é da turma e avisa quando ela já está presente; sem a lista, ele grava o que o professor digitar, e o banco junta as pontas depois.

O painel é a tela em que o professor passa a aula inteira, e foi por isso que o desenhamos assim. Ele responde à pergunta que o professor tem na cabeça, que é quantos alunos já registraram presença, e mostra que o último crachá encostado foi lido, com o nome de quem o encostou. O aluno vê o LED e ouve o bip no aparelho; o professor vê a mesma coisa escrita, e pode conferir na hora o aluno que reclamar.

### O relatório, o envio e o encerramento

O relatório lista os presentes, com a matrícula e a hora da leitura, e os faltantes da turma (Figura 5). Na aula sem internet, em que o aparelho não tinha a lista, os crachás lidos aparecem à parte, sem nome, com um aviso de que não descontam ninguém dos faltantes, para a conta não parecer errada.

**Figura 5: O relatório, com os presentes e os faltantes**

![O relatório, com os presentes e os faltantes](../../../assets/dashboard/2026-10-13-portal-5-relatorio.png)

Fonte: elaborado pelos autores (2026).

O relatório tem duas saídas. O Compartilhar baixa o arquivo no celular do professor, sem passar pela internet, e serve para o dia em que o envio falhar. O botão "Enviar e fechar a chamada" manda a chamada ao banco; confirmado o envio, o leitor para de registrar, e o painel passa a dizer que a chamada está fechada (Figura 6). Se o professor tocar no botão outra vez, o aparelho responde que a chamada já tinha sido enviada, e nada se duplica. Se o envio falhar, a chamada continua aberta e registrando, e o professor pode tentar de novo ou salvar o relatório pelo Compartilhar.

**Figura 6: O painel depois do envio, com a chamada fechada**

![O painel depois do envio, com a chamada fechada](../../../assets/dashboard/2026-10-13-portal-6-chamada-enviada.png)

Fonte: elaborado pelos autores (2026).

O Encerrar pede confirmação (Figura 7), apaga do aparelho a lista da turma e as presenças da aula e devolve a tela de login, pronta para o próximo professor. Ele vale em qualquer tela depois do login, inclusive durante a busca da turma, que ele interrompe. É por esse botão que a lista com o nome e a matrícula dos alunos sai do aparelho ao fim da aula.

**Figura 7: A confirmação do Encerrar**

![A confirmação do Encerrar](../../../assets/dashboard/2026-10-13-portal-7-encerrar.png)

Fonte: elaborado pelos autores (2026).

### As regras que o portal segue

Nenhuma tela avança sem o aparelho confirmar. Quando ele não responde, ou quando recusa, o portal mostra a falha, libera o botão e deixa a tela onde está, para o professor não ficar com a impressão de que o aparelho obedeceu. Essa é a regra que mais mudou o desenho das telas: cada botão do painel espera a resposta antes de mudar qualquer coisa, e a tela do relatório espera a resposta de um pedido antes de fazer outro, porque o aparelho atende um pedido de cada vez e a tela encheria a fila dele de pedidos que ninguém mais iria ler.

Três situações da sala de aula também entraram no portal. Se a página for recarregada com a chamada aberta, o portal pergunta o estado ao aparelho e volta direto ao painel, sem pedir a senha de novo, porque o professor que fecha a janela sem querer não deve perder a chamada. Se outra conta entrar com um professor já logado, ela assume o painel enquanto a chamada não começou; depois do Iniciar, a chamada continua de quem a iniciou, e cada lançamento manual guarda quem estava logado ao lançar, porque recusar o segundo login prenderia o aparelho ao professor que saiu da sala sem encerrar. E, depois de uma queda de energia, a chamada volta como estava, mas o aparelho só torna a ler crachá quando um professor entrar de novo e lhe devolver a hora; até lá, o portal mostra a chamada aberta e o aparelho recusa o lançamento manual e o envio, porque um registro sem hora faria o banco recusar o relatório inteiro.

### O que já rodou na placa

Até 20/10, o portal servido pelo ESP32, falando com o aparelho de verdade, rodou na placa do login à chamada: a página de login que abre sozinha, a senha recusada e a aceita, a busca da lista da turma, a entrada no modo offline quando não há internet, o Iniciar, a leitura dos crachás nos dois modos, o lançamento manual e a tela do relatório. O envio ao banco, o encerramento e a volta depois de uma queda de energia esperam as duas rodadas que faltam. O resultado de cada item, com os números, está na seção dos testes.

## A página de histórico

Ao fim da aula, o aparelho envia a chamada a um banco de dados na nuvem, na plataforma Supabase, e a página de histórico lê esse banco pela internet. Ela não conversa com o aparelho e não altera nada: as permissões do banco dão a ela o direito de ler, e nenhum outro. O endereço é público, mas a leitura exige e-mail e senha, e sem login o banco não entrega aula nenhuma.

Escrevemos a página porque a apresentação precisa mostrar um sistema, e não só um leitor de crachá. O portal do professor morre com a aula: ao Encerrar, o aparelho apaga tudo o que guardava. O que resta da aula fica no banco, e sem uma tela que o leia esse banco seria uma afirmação nossa, e não uma coisa que o professor pode abrir e conferir.

A página está no ar em `https://caioplaninschek.github.io/presente/` desde 13/10. As figuras desta seção são desse dia, com as aulas de teste que o banco então tinha. Em 15/10 esvaziamos o banco, antes das rodadas na placa, para que as primeiras aulas de verdade não se misturassem às inventadas; até a rodada do envio ao banco, a página abre sem aula nenhuma.

### O login e a lista das aulas

A página abre pedindo e-mail e senha (Figura 8). As contas são criadas à mão, e o cadastro público está desligado.

**Figura 8: A tela de login da página de histórico**

![A tela de login da página de histórico](../../../assets/dashboard/2026-10-13-historico-1-login.png)

Fonte: elaborado pelos autores (2026).

A lista mostra as aulas da mais recente para a mais antiga, cada uma com a data e a hora, o professor, a turma, a disciplina, a situação e o modo (Figura 9). A situação diz se a aula está aberta, foi finalizada ou foi cancelada, e o modo diz se o aparelho tinha a lista da turma, caso em que a aula aparece como "Com internet", ou não a tinha, caso em que aparece como "Sem internet".

**Figura 9: A lista das aulas registradas no banco**

![A lista das aulas registradas no banco](../../../assets/dashboard/2026-10-13-historico-2-lista.png)

Fonte: elaborado pelos autores (2026).

### Os presentes e os faltantes de uma aula

Ao abrir uma aula, a página mostra os presentes, cada aluno uma vez só, com a matrícula e o horário da primeira leitura, e os faltantes da turma. A presença lançada à mão vem marcada, com o motivo e com quem a lançou. As Figuras 10 e 11 são o começo e o fim de uma aula de teste registrada sem internet, com 34 presentes, um deles lançado à mão porque o aluno esqueceu o crachá, e 6 faltantes.

**Figura 10: O começo de uma aula, com os presentes**

![O começo de uma aula, com os presentes](../../../assets/dashboard/2026-10-13-historico-3-aula-topo.png)

Fonte: elaborado pelos autores (2026).

**Figura 11: O fim da mesma aula, com a presença lançada à mão e os faltantes**

![O fim da mesma aula, com a presença lançada à mão e os faltantes](../../../assets/dashboard/2026-10-13-historico-4-aula-fim.png)

Fonte: elaborado pelos autores (2026).

Nas aulas sem internet, o aparelho gravou só o número do crachá, e é o banco que liga esse número ao aluno no envio. O crachá que o banco não conhece aparece na página sem nome, e a página avisa que ele não desconta ninguém dos faltantes.

### A chave do banco e a publicação

Nenhuma credencial do banco fica no repositório. A cada publicação, o GitHub monta o arquivo de configuração da página com o endereço do banco e a chave publicável dele, guardados como segredos do repositório, e só então põe a página no ar. Essa chave fica visível para quem abre a página, e aceitamos esse custo com uma compensação: sozinha, ela não lê nada, porque o banco só libera a leitura das tabelas depois do login.
