> **Seção do relatório técnico: as limitações e os caminhos descartados (#70).** Quem escreve: Cauã. O que entra: a conexão segura que não autentica o servidor (R13); o que consegue fazer quem extrair a chave do aparelho (R33); a chave visível na página de histórico (R50); o caminho de evolução descartado por razão acadêmica, o de deixar a internet com o celular do professor (R14); os limites declarados da R76, da R78 e da R79; e os cortes do §1. O modo offline que aceita todo crachá (R3) e o banco gratuito que pausa (R60) não estão no enunciado da issue e esperam a resposta do Caio nela; se ele disser que não, a subseção dos dois sai. O caminho descartado está escrito pelo estado do rádio depois da R45, e não pela frase original da R14, que fala da troca de rádio que deixou de existir. Fontes: essas decisões e o §1 da `docs/spec.md`. Extensão: em branco até o professor responder sobre o número de páginas (ver o `indice.md`); hoje o texto tem um quadro. Ficam de fora, por serem de outras seções: o que a chave pode fazer no banco, descrito como arquitetura (introdução e arquitetura, #66), e os números e os testes que não rodaram (testes, #69). A seção do firmware (#67) já cita cada limite declarado em uma frase, e esta não repete a descrição: diz até onde ele vai e por que o aceitamos. Este bloco sai na consolidação.

# LIMITAÇÕES E CAMINHOS DESCARTADOS

Cada limitação abaixo foi aceita numa decisão de projeto, e não descoberta depois. Para cada uma, dizemos o que pode acontecer, até onde vai o dano e por que a aceitamos.

## A chave do aparelho e a da página de histórico

O aparelho fala com o banco por uma chave publicável, gravada na memória da placa, que não tem privilégio em tabela nenhuma e só executa duas funções: a que devolve a lista de uma turma e a que grava o relatório de uma chamada. Quem extrair essa chave da placa consegue pedir a lista de uma turma cujo código conheça e ler o nome, a matrícula e o crachá dos alunos ativos dela, e o código da turma real está no repositório do projeto, que é público. Consegue também enviar o relatório falso de uma aula nova. Não consegue ler a lista de turmas nem o histórico, nem apagar ou alterar o que já está gravado. O pior caso é bagunçar uma aula, que é o limite que aceitamos ao definir o banco de dados (#1).

A página de histórico, publicada pelo GitHub Pages, roda no navegador com uma chave publicável dentro dela. A chave não entra no repositório, porque o GitHub a põe na página na hora de publicá-la, a partir de um segredo, mas fica visível para quem abre a página. O custo é o mesmo da chave do aparelho, agora ao alcance de qualquer um que tenha o endereço, sem precisar da placa. A leitura do histórico continua exigindo login: o cadastro público do banco fica desligado, e as contas da página são criadas à mão, porque, com o cadastro ligado, qualquer um com a chave criaria uma conta e leria o histórico inteiro.

## A conexão segura que não autentica o servidor

A conexão do aparelho com o banco é criptografada, mas o aparelho não confere o certificado do servidor. Quem controlasse a rede do hotspot poderia, em teoria, se pôr entre o aparelho e o banco e se passar pelo banco. Não fixamos o certificado no aparelho porque o Supabase não usa uma autoridade certificadora só e troca de emissor sem aviso: em 06/09, encontramos endereços do mesmo serviço com certificados da Let's Encrypt, da Amazon e do Google. Um certificado fixo deixaria de valer sozinho, e o aparelho perderia o banco sem que ninguém soubesse por quê. Julgamos o risco proporcional a um protótipo acadêmico, em que o hotspot é o celular de um integrante do grupo e a chave que trafega é publicável e descartável.

## O modo offline e o banco que pausa

Quando a lista da turma não chega no login, o aparelho entra no modo offline e aceita todo crachá lido, porque não tem como saber quem é da turma. Ele registra só o número do crachá, e o banco liga cada número à matrícula do dono quando recebe o relatório; o crachá sem dono no cadastro fica com a matrícula vazia. No modo offline, o verde quer dizer que o aparelho leu o crachá, e quem decide, depois, se o crachá é da turma é o banco. Aceitamos isso porque, recusando o crachá sem a lista, o aparelho tiraria a chamada da aula sem internet, e no modo offline nenhum toque se perde.

O banco roda no plano gratuito do Supabase, que pausa o projeto depois de cerca de sete dias sem uso. Com o banco pausado, a busca da turma falha, e o aparelho entra no modo offline sem aviso, como se não houvesse internet; o sintoma é o endereço do banco que não abre, como aconteceu em 21/09. Decidimos não fazer o agendamento que chamaria o banco a cada três dias para mantê-lo acordado, porque reativar o banco pelo painel do Supabase, no celular, é simples, e o agendamento seria trabalho a mais. Só o Caio tem acesso ao painel, e quem vê o sintoma o avisa. A lista do que conferir antes da apresentação inclui abrir o painel e, se o projeto estiver pausado, retomá-lo.

## Os limites declarados do firmware

Sete limites do firmware ficaram declarados, sem código para eles. A seção do firmware diz cada um ao lado da decisão que o gerou; aqui dizemos até onde eles vão e por que os aceitamos.

**O login de outra conta com a chamada iniciada.** Outra conta pode entrar com um professor já logado, e a chamada já iniciada continua de quem a iniciou. Se o professor seguinte, ainda assim, der a aula dentro dela, as presenças vão para a sessão do anterior, com o horário de início dela. A decisão resolve o caso comum, o do professor que sai da sala sem encerrar: recusar a conta seguinte prenderia o aparelho a ele, e a chamada que ele não enviou ficaria fora do banco. O painel mostra quem iniciou a chamada, para que o professor seguinte não a tome pela sua, e o caminho previsto é ele enviar a chamada que ficou, encerrá-la e abrir a dele.

**Dois Iniciar no mesmo minuto.** A identificação da chamada leva a turma, a data e a hora do Iniciar, com resolução de minuto. Dois Iniciar no mesmo minuto dão a mesma identificação, e o segundo envio recebe do banco a resposta de que a chamada já estava gravada, sem gravar nada. No teste de dois professores em sequência, o segundo Iniciar espera o minuto virar.

**A resposta do banco que se perde.** Se o banco grava o envio e a resposta se perde no caminho, o aparelho entende que não chegou ao banco, e a chamada segue aberta, registrando. O segundo envio recebe a resposta de que a chamada já estava gravada. Os toques do intervalo ficam fora do banco, embora continuem no relatório do Compartilhar, e o horário de fim no aparelho difere do que o banco guardou. Corrigir isso exigiria que o banco aceitasse mudar uma chamada já gravada, o contrário da regra que impede o duplo clique no Enviar de duplicar o relatório, e o Caio decidiu manter a regra, em 14/10.

**O pior caso dos tempos do envio.** O envio usa os mesmos tempos de partida da busca da turma, que somam 20 s, mas o pior caso passa disso, porque a conexão, a negociação segura, a busca do endereço do banco e cada leitura têm prazo próprio. O portal espera até 25 s pela resposta. Quanto o envio leva na placa é o que a rodada do relatório na placa (#48) mede, e o resultado está na seção de testes.

**Os três limites da volta depois de uma queda de energia.** Se o Encerrar não consegue apagar os dados da chamada, falha que ele só avisa no monitor serial, o próximo reinício volta à chamada encerrada, com o professor dela no painel, e é preciso encerrá-la de novo. A chamada online cuja lista da turma não volta do disco segue no modo offline, e o relatório sai como offline, com a matrícula nos eventos de antes da queda e só o crachá nos de depois. Se a conta do professor que iniciou a chamada sai do aparelho entre a queda e a volta, o relatório continua com a identificação de quem a iniciou, e quem entra assume o painel. Os três dependem de uma falha da memória da placa ou de um arquivo trocado no meio da aula, e tratá-los com código mudaria o envio e o encerramento, ou as rotas entre o portal e o aparelho, na semana em que o código fechava.

## Os cortes do protótipo

O protótipo deixou de fora oito funções, cada uma por uma razão declarada, e o Quadro 3 as reúne.

**Quadro 3: Os cortes do protótipo**

| Corte | Razão |
|---|---|
| A integração real com o sistema acadêmico da UVA | Não temos acesso ao sistema; o banco no Supabase faz esse papel no protótipo |
| O celular do aluno no lugar do crachá, pelo NFC do aparelho pessoal | Fica como caminho de evolução, sem construção |
| A biometria e a catraca | O custo, a LGPD, que trata a biometria como dado sensível, e o escopo |
| O controle de saída da sala | Duplicaria o estado e as falhas sem resolver o problema que motivou o projeto; a divergência com o Canvas da Entrega 02 foi declarada na Entrega 03 |
| Um servidor próprio, com Node ou Python e PostgreSQL | Substituído pelo Supabase, que expõe o banco sem servidor para escrever |
| O segundo sensor e a detecção de violação da caixa | O professor dispensou a exigência de dois sensores, e o sensor de vibração saiu do projeto |
| A troca da senha padrão no primeiro uso e a configuração do hotspot pelo portal | As contas chegam cadastradas pelo grupo, e o hotspot principal e o de reserva vêm do `secrets.h`; as duas funções acrescentariam telas e escrita na memória da placa sem mudar o que a chamada faz, e, sem a configuração pelo portal, trocar o celular do hotspot exige gravar o aparelho de novo |
| A escolha da turma no aparelho | A turma e a disciplina ficam fixas na configuração; em produção, viriam da grade horária da instituição |

Fonte: elaborado pelos autores (2026).

## O caminho de evolução descartado

Consideramos tirar a internet do ESP32 e deixar o celular do professor falar com o banco, de modo que o aparelho nunca saísse da própria rede. O ESP32 não precisaria entrar no hotspot nem abrir conexão segura, e sairiam do firmware os custos que vêm dela: a memória que a negociação segura pede; os cerca de 4 KB que cada conexão segura que falha deixa presos, que limitam a busca da turma e o envio a uma tentativa; e a busca do hotspot de canal em canal, que obriga o aparelho a sair do hotspot no fim de cada busca. Descartamos o caminho por razão acadêmica: numa disciplina de Sistemas Embarcados, a comunicação do ESP32 com a nuvem é a competência avaliada. Ele fica registrado como caminho de evolução, ao lado do celular do aluno no lugar do crachá, que citamos e não construímos.
