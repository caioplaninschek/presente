# 6 LIMITAÇÕES E CAMINHOS DESCARTADOS

Cada limitação abaixo foi aceita numa decisão de projeto, e não descoberta depois.

## 6.1 As chaves e a conexão com o banco

A chave publicável gravada na placa não tem privilégio em tabela nenhuma e só executa as duas funções do banco. Quem a extrair consegue pedir a lista de uma turma cujo código conheça, com o nome, a matrícula e o crachá dos alunos ativos, e o código da turma real está no repositório do projeto, que é público. Consegue também enviar o relatório falso de uma aula nova, mas não consegue ler a lista de turmas nem o histórico, nem apagar ou alterar o que já está gravado; o pior caso é bagunçar uma aula. A chave da página de histórico tem o mesmo custo, ao alcance de qualquer um que tenha o endereço da página, e a leitura do histórico continua exigindo login, porque o cadastro público do banco fica desligado e as contas da página são criadas à mão.

A conexão do aparelho com o banco é criptografada, mas o aparelho não confere o certificado do servidor, e quem controlasse a rede do hotspot poderia, em teoria, se passar pelo banco. Não fixamos o certificado no aparelho porque o Supabase troca de emissor sem aviso, e um certificado fixo deixaria de valer sozinho, sem que ninguém soubesse por quê. Julgamos o risco proporcional a um protótipo acadêmico, em que o hotspot é o celular de um integrante do grupo e a chave que trafega é publicável e descartável.

## 6.2 O modo offline e o banco que pausa

Sem a lista da turma, o aparelho aceita todo crachá lido, e o verde quer dizer que ele leu o crachá; quem decide depois se o crachá é da turma é o banco. Aceitamos isso porque, recusando o crachá sem a lista, o aparelho tiraria a chamada da aula sem internet, e no modo offline nenhum toque se perde. O banco roda no plano gratuito do Supabase, que pausa o projeto depois de cerca de sete dias sem uso; com o banco pausado, a busca da turma falha, e o aparelho entra no modo offline sem aviso. Reativar o banco pelo painel do Supabase é simples, e o checklist do dia da exposição incluía esse passo.

## 6.3 Os limites declarados do firmware

Oito limites do firmware ficaram declarados, sem código para eles (Quadro 9). O toque que grava antes do sinal é o único que apareceu nos testes, e o Caio aceitou o limite porque dar o sinal antes de gravar faria o aluno ver o verde de uma presença que uma queda de energia ainda podia apagar, e porque mexer na gravação a uma semana da exposição arriscaria um programa que já tinha passado nos testes. A resposta que se perde vem de uma regra que o Caio decidiu manter em 14/10, a que impede o duplo clique no Enviar de duplicar o relatório, e os três limites da volta depois da queda dependem de uma falha da memória da placa ou de um arquivo trocado no meio da aula.

**Quadro 9: Os limites declarados do firmware**

| Limite | O que pode acontecer | Por que aceitamos |
|----|----|----|
| O login de outra conta com a chamada iniciada | Se o professor seguinte der a aula dentro da chamada do anterior, as presenças vão para a sessão do outro | Recusar a conta seguinte prenderia o aparelho ao professor que sai da sala sem encerrar, e a chamada que ele não enviou ficaria fora do banco; o painel mostra quem iniciou a chamada |
| Dois Iniciar no mesmo minuto | A identificação da chamada tem resolução de minuto, e o segundo envio não grava nada | No teste de dois professores em sequência, o segundo Iniciar espera o minuto virar |
| A resposta do banco que se perde | O banco grava o envio, mas a chamada segue aberta, e os toques seguintes ficam fora do banco, embora continuem no relatório do Compartilhar | Corrigir isso exigiria que o banco aceitasse mudar uma chamada já gravada, o contrário da regra que impede o duplo clique no Enviar de duplicar o relatório, que o Caio decidiu manter em 14/10 |
| O pior caso dos tempos do envio | A soma dos prazos passa dos 20 s | O portal espera até 25 s pela resposta |
| O toque que grava antes do sinal | Três toques medidos passaram do tempo pedido entre o toque e o sinal, dois em 20/10 e um em 27/10, e, em 26/10, pelo relato do Caio, um cartão passou de novo; a suspeita, não confirmada, é a memória da placa apagando um bloco antes de gravar | Dar o sinal antes de gravar faria o aluno ver o verde de uma presença que uma queda de energia ainda podia apagar, e mexer na gravação a uma semana da exposição arriscaria um programa que já tinha passado nos testes |
| O Encerrar que não apaga os dados | O próximo reinício volta à chamada encerrada, e é preciso encerrá-la de novo | Os três limites da volta depois da queda dependem de uma falha da memória da placa ou de um arquivo trocado no meio da aula, e tratá-los com código mudaria o envio e o encerramento, ou as rotas entre o portal e o aparelho, na semana em que o código fechava |
| A lista da turma que não volta do disco depois da queda | A chamada segue no modo offline, e os eventos de depois da queda saem só com o crachá | O mesmo dos outros limites da volta depois da queda |
| A conta do professor removida entre a queda e a volta | O relatório continua com a identificação de quem iniciou a chamada | O mesmo dos outros limites da volta depois da queda |

Fonte: elaborado pelos autores (2026).

## 6.4 Os cortes do protótipo

O protótipo deixou de fora oito funções, cada uma por uma razão declarada (Quadro 10).

**Quadro 10: Os cortes do protótipo**

| Corte | Razão |
|----|----|
| A integração real com o sistema acadêmico da UVA | Não temos acesso ao sistema; o banco no Supabase faz esse papel no protótipo |
| O celular do aluno no lugar do crachá | Fica como caminho de evolução, sem construção |
| A biometria e a catraca | O custo, a LGPD, que trata a biometria como dado sensível, e o escopo |
| O controle de saída da sala | Duplicaria o estado e as falhas sem resolver o problema que motivou o projeto |
| Um servidor próprio | Substituído pelo Supabase, que expõe o banco sem servidor para escrever |
| O segundo sensor e a detecção de violação da caixa | O professor dispensou a exigência de dois sensores, e o sensor de vibração saiu do projeto |
| A troca da senha padrão no primeiro uso e a configuração do hotspot pelo portal | As contas chegam cadastradas pelo grupo, e o hotspot principal e o de reserva vêm do `secrets.h`; as duas funções acrescentariam telas e escrita na memória da placa sem mudar o que a chamada faz, e, sem a configuração pelo portal, trocar o celular do hotspot exige gravar o aparelho de novo |
| A escolha da turma no aparelho | A turma fica fixa na configuração; em produção, viria da grade horária da instituição |

Fonte: elaborado pelos autores (2026).

## 6.5 O caminho de evolução descartado

Consideramos tirar a internet do ESP32 e deixar o celular do professor falar com o banco, de modo que o aparelho nunca saísse da própria rede e o firmware dispensasse a conexão segura, com a memória que ela pede. Descartamos o caminho por razão acadêmica: numa disciplina de Sistemas Embarcados, a comunicação do ESP32 com a nuvem é a competência avaliada. Ele fica registrado como caminho de evolução, ao lado do celular do aluno no lugar do crachá.
