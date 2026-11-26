# 5 OS TESTES E OS RESULTADOS

Testamos o Presente! na placa, com o aparelho montado, seguindo o plano de testes da especificação, que dá a cada teste um critério de aprovação (Quadro 7). Só o comportamento visto de fora entra no teste: o que o LED e o buzzer fazem, o que o portal mostra, o que sai no relatório e o que o monitor serial registra. O plano pede que todo teste deixe evidência gravada no repositório, em `docs/assets/testes/`, e o que passou só pelo relato do grupo ou do Caio, sem evidência gravada, está declarado como tal. O tempo do toque ao sinal é medido pelo próprio programa e é um piso, porque o tempo em que o crachá espera no campo até o programa ir buscá-lo não se mede de dentro do aparelho.

**Quadro 7: Os testes do plano, com o esperado e o obtido**

| Teste | O que se espera | O que obtivemos | Evidência |
|----|----|----|----|
| 1 · Três toques seguidos | Um registro só, e o sinal em menos de 200 ms | Passou em 22/09, menos no som, porque o buzzer de então só estalava, e na aula de 20/10; em outros toques de 20/10, dois que registraram presença, um em casa e um na aula, passaram dos 200 ms, e, na rodada de 26/10, pelo relato do Caio, um cartão levou cerca de 300 ms; na exposição de 27/10, um de quatro toques levou 258,4 ms numa chamada, e os seis de outra ficaram abaixo de 200 ms (Quadro 8) | O programa do sinal de presença (#26): `encostou-2026-09-22.mp4` e `encostou-monitor-serial-2026-09-22.jpg`. A rodada do pacote 3 na placa (#46): `teste-1-tres-toques-2026-10-20.mp4`. A exposição: `monitor-chamada-2026-10-27-1911-tempos.jpg` e `monitor-segunda-chamada-2026-10-27-19h34-conferencias.jpg` |
| 2 · Crachá de 7 bytes | O registro pelo número do crachá | Saiu do plano em 10/10, porque os crachás do projeto têm número de 4 bytes | O cadastro dos cartões novos (#40) |
| 3 · Crachá fora da turma | Vermelho longo, e nada gravado | Passou na aula de 20/10, com o cartão sem cadastro e o do kit | A rodada do pacote 3 na placa (#46): `teste-3-cracha-sem-cadastro-2026-10-20.mp4` e `dois-crachas-fora-da-turma-2026-10-20.mp4` |
| 4 · Modo offline | Todo crachá aceito e o relatório íntegro | Passou em casa, em 20/10, e, pelo relato do grupo, na aula do mesmo dia | A rodada do pacote 3 na placa (#46), no comentário do João. A rodada do pacote 4 na placa (#48): `enviar-chamada-offline-2026-10-20.mp4` |
| 5 · Enviar sem e com internet | O novo envio sobe o mesmo relatório, sem duplicar | Passou pelo relato do grupo em 20/10; o serial guarda o envio com internet | A rodada do pacote 4 na placa (#48): `serial-2026-10-20.txt` e `enviar-e-cracha-depois-do-envio-2026-10-20.mp4` |
| 6 · Dois professores em sequência | Duas chamadas, sem mistura | Passou pelo relato do grupo em 20/10 | A rodada do pacote 4 na placa (#48) |
| 7 e 7b · Alternância do rádio | A rede do aparelho volta a cada troca | Saíram do plano em 22/09, com a adoção do modo simultâneo | O veredito do rádio (#19) |
| 7c · Painel durante o pulo de canal | O painel continua aberto, em iPhone e Android | Passou em 22/09, pelo relato do grupo; em 20/10, não foi observado, porque o canal não mudou | O teste do painel (#18) e a rodada do pacote 2 na placa (#44) |
| 7d · Memória antes da conexão segura | 40 KB ou mais | Passou em todas as medições, de 236,5 KB em 21/09 a 204,3 KB em 27/10; em 26/10, ficou dentro do limite, pelo relato do Caio, sem o número (Quadro 8) | O teste de memória (#17) e `serial-2026-10-20.txt`. A exposição: `monitor-chamada-2026-10-27-1911-painel.jpg` |
| 8 · 30 toques com uma queda de energia | Nenhuma duplicata nem perda | Obtido em parte em 22/09, com dois crachás; passou pelo relato do grupo em 20/10 | O programa da persistência (#27) e a rodada completa (#50) |
| 9 · Caixa fechada | O cartão lido pela tampa, com o verde e o bip | Em 06/10, pela informação do Caio, o cartão foi lido de tampa fechada, e o resto do critério ficou sem registro; passou em 20/10, pelo relato do grupo; com a tampa pintada, o cartão foi lido com o verde e o bip em 25/10, pela informação do Caio, e o teste passou em 26/10, pelo relato dele; na exposição de 27/10, a caixa fechada registrou os cartões das chamadas de demonstração | A montagem da caixa (#33): `entrega-06-video-2026-10-05.mp4`. O furo do LED (#58): `led-pelo-furo-tampa-fechada-2026-10-20.jpg`. A exposição: `monitor-chamada-2026-10-27-1911-crachas.jpg` |

Fonte: elaborado pelos autores (2026).

## 5.1 As rodadas na placa

Os primeiros testes rodaram em 21 e 22/09, com um programa de teste por experimento, e entraram na Entrega 05, aprovada na demonstração de 22/09; o João gravou e rodou os programas. Nesses dias, o teste de memória e o do painel decidiram o rádio. Em 05/10, com a caixa montada, o cartão foi lido através da tampa fechada. Em 06/10, já com o ESP32 preso, foi feito o teste completo com a caixa fechada, e, pela informação do Caio, o cartão foi lido de novo pela tampa; o resto do teste ficou sem registro. Em 12/10, o Caio testou em casa, de modo informal, a busca da lista da turma, que levou uns 8 s com o hotspot ligado.

Com o programa completo na placa, o João rodou em casa a bateria dos pacotes 1 a 3 do firmware, do portal à chamada (#42, #44 e #46), em 19 e 20/10, e o grupo rodou na aula de 20/10 o que faltava: os três toques seguidos e o crachá fora da turma (testes 1 e 3), com a lista da turma, o envio ao banco, os dois professores em sequência e a volta depois de uma queda de energia. O monitor serial da aula só foi copiado até as 20h14, e o que rodou depois ficou nos vídeos ou só no relato do grupo. Em 25/10, pela informação do Caio, o cartão foi lido pela tampa pintada de prata, sem raspar a tinta, e, em 26/10, o Caio, o João e o Igor repetiram a rodada completa com a caixa pintada e fechada; pelo relato do Caio, passou tudo, fora um cartão que levou cerca de 300 ms até o sinal, e a rodada não deixou evidência guardada.

Na exposição de 27/10, a caixa fechada rodou o firmware completo diante dos visitantes, com a lista da turma, e o monitor serial ficou aberto no notebook. Nos quadros que filmamos da tela, a chamada `4172CMPN6A_P1-20261027-1911` tem os 40 alunos na lista, a busca da turma em 4,3 s, quatro crachás registrados e uma presença lançada à mão, e outra chamada, perto das 19h34, tem seis toques. Os quadros estão em `docs/assets/exposicao/`.

## 5.2 As medições

Nos toques medidos que registraram presença, o tempo do toque ao sinal ficou abaixo de 100 ms, fora três, e a memória livre antes da conexão segura não desceu de 204,3 KB nas rodadas que a anotaram (Quadro 8).

**Quadro 8: As medições na placa**

| Rodada | Toque ao sinal, nos toques que registram | Acima de 200 ms | Memória antes da conexão segura |
|----|----|----|----|
| 21/09, teste de memória | Não medido | Não medido | 236,5 KB |
| 22/09, programa do sinal de presença | 29,2 e 29,3 ms | Nenhum | Não medida |
| 12/10, teste informal em casa | Não medido | Não medido | De 211,7 a 212,4 KB |
| 20/10, bateria em casa | De 41,9 a 96,0 ms, fora o de 352,5 ms | 1, com 352,5 ms | De 208,5 a 209,5 KB |
| 20/10, aula | De 42,8 a 87,3 ms, fora o de 295,2 ms | 1, com 295,2 ms | De 205,5 a 210,0 KB |
| 26/10, rodada antes da exposição | Não anotado | 1, com cerca de 300 ms, pelo relato do Caio | Dentro do limite, pelo relato do Caio, sem o número |
| 27/10, exposição, chamada das 19h11 | De 46,8 a 54,3 ms, fora o de 258,4 ms | 1, com 258,4 ms | 204,3 KB |
| 27/10, exposição, chamada perto das 19h34 | Os seis abaixo de 200 ms; os valores não se leem nos quadros | Nenhum | 207,4 KB |

Fonte: elaborado pelos autores (2026).

Nos três toques medidos acima de 200 ms, a maior parte do tempo ficou na decisão, que inclui a gravação do registro na memória da placa: em 27/10, foram 229,0 dos 258,4 ms. Em 20/10, os toques seguintes da mesma chamada voltaram a ficar abaixo de 100 ms; em 27/10, o toque lento foi o último da chamada, e, na chamada seguinte, os seis toques ficaram abaixo de 200 ms. A lista dos 40 alunos ocupou 4365 dos 8192 bytes reservados para ela, e, na exposição, a busca da turma levou 4,3 s, medida pelo relógio do computador.

## 5.3 O que não passou, não rodou ou ficou sem registro

Três toques medidos que registraram presença passaram dos 200 ms do teste dos três toques seguidos (teste 1), dois em 20/10 e um na exposição de 27/10, e, na rodada de 26/10, pelo relato do Caio, um cartão levou cerca de 300 ms; o Caio decidiu não mexer no firmware, e o limite está declarado nas limitações. O painel durante o pulo de canal ficou como não observado na aula de 20/10, porque os dois hotspots estavam no canal da rede do aparelho, e o pulo também não aconteceu na bateria em casa; a rodada em que o teste passou continua sendo a de 22/09, sem registro do monitor serial.

Na aula de 20/10, o modo offline, o Enviar sem e com internet, os dois professores em sequência, os 30 toques com uma queda de energia e a caixa fechada (testes 4, 5, 6, 8 e 9), o login de outra conta com a chamada aberta e a volta depois da queda passaram pelo relato do grupo, porque o monitor serial só foi copiado até as 20h14 e os relatórios baixados não foram guardados. A leitura pela tampa pintada, em 25/10, e a rodada de 26/10 existem só pela informação e pelo relato do Caio, sem nada guardado. Os registros do monitor serial da bateria em casa estão só nos comentários das rodadas dos pacotes 1 a 3 na placa (#42, #44 e #46). Não rodaram a leitura do crachá de 7 bytes com registro de presença e a calibração da espera pelo hotspot de reserva, e o tempo do Enviar não foi anotado. Da exposição de 27/10, há só os quadros do monitor filmados no notebook: os números das chamadas saem deles, e o que não se lê nos quadros, como os tempos da segunda chamada, ficou sem registro.
