# Pauta do grupo — o que falta decidir

Documento para **conversar com a equipe**, em linguagem direta. O detalhe técnico de cada assunto vive nos tickets em `.scratch/presente/issues/`; aqui está a versão para debate.

**Como ler.** São oito pontos. Cada um traz a situação em poucas linhas, **a minha sugestão** e o motivo dela. As sugestões não são decisão fechada — são um ponto de partida para vocês reagirem. Discordar com argumento é exatamente o que se espera; concordar rápido também vale, e economiza reunião.

> Isto é um documento de conversa, não um registro. Quando o grupo decidir, a decisão vai para a `spec.md` e esta pauta pode ser jogada fora.

> ## ✅ Encerrada em 10/09/2026
>
> Os oito pontos foram respondidos pelo grupo nas issues #1 a #4 do GitHub. As decisões estão na `spec.md`, rodada 7 (R19–R22); esta pauta fica só como registro de como as perguntas foram feitas.
>
> | # | Assunto | Onde foi decidido |
> |---|---|---|
> | 1–3 | Crachá por aluno, aluno em várias turmas, aluno que sai | issue #1 → R20 |
> | 4–6 | Poderes da chave, duplo Enviar, aula sem internet | issue #1 → R20 (o duplo Enviar sai do `estado` da sessão) |
> | 7 | Tela de histórico | issue #2 → R21, dono Igor |
> | 8 | Voltar ao painel depois da troca de rede | issue #3 → R19 — **a premissa estava errada:** `WIFI_AP_STA` existe, e o projeto passa a ter Plano A e Plano B, a decidir na bancada |
> | Parte 4 | Material e caixa | issue #4 → R22 — material completo, com o João; caixa adiada |

---

# Parte 1 — O banco de dados na nuvem

**Do que estamos falando.** O aparelho fica sem internet durante quase toda a aula. Ele só usa internet em **dois momentos**: no começo, quando o professor faz login, para baixar a lista de quem é daquela turma; e no fim, quando o professor aperta Enviar, para mandar o resultado.

Do outro lado dessa internet precisa existir alguém guardando essas informações. Escolhemos o **Supabase**, um banco de dados gratuito na nuvem. Ele faz dois papéis: **finge ser o sistema da UVA** (que não temos acesso para integrar de verdade) e serve para mostrar os resultados acumulados na apresentação.

A ideia é guardar três "fichários": **alunos** (matrícula, nome, número do crachá), **aulas** (dia, turma, professor, começo e fim) e **presenças** (quem, em qual aula, a que horas, pelo crachá ou lançado na mão).

Seis pontos a fechar.

## 1. Um aluno pode ter mais de um crachá ativo?

Alguém perde a tag e ganha outra. O número antigo continua valendo?

**Minha sugestão: um crachá ativo por aluno.** Perdeu, a gente troca o número no cadastro e o antigo deixa de funcionar na hora.

**Por quê:** dois crachás ativos significam que o aluno pode emprestar um e ainda registrar a própria presença com o outro. Isso traz de volta exatamente a fraude que o projeto existe para evitar. Além disso, cadastro é coisa nossa (no cenário real, da instituição) — o professor nunca cadastra ninguém, então trocar um número é rápido.

**O que quero de vocês:** alguém vê caso real em que dois crachás ativos fazem falta?

## 2. Um aluno pode estar em mais de uma turma?

No mundo real, obviamente sim — todo mundo cursa várias disciplinas. A pergunta é se o nosso protótipo precisa disso.

**Minha sugestão: sim, e separando "aluno" de "quem está em qual turma".** O aluno é cadastrado uma vez, pela matrícula, e existe uma listinha à parte dizendo em quais turmas ele está.

**Por quê:** custa uma tabela de duas colunas e evita cadastrar a mesma pessoa três vezes, com três números de crachá diferentes, que é como isso quebra. E é a diferença entre a apresentação parecer um sistema e parecer uma gambiarra de demonstração.

**O que quero de vocês:** concordam que vale a tabelinha a mais, ou preferem simplificar e assumir uma turma só no protótipo?

## 3. E o aluno que sai da turma no meio do semestre?

Trancou, mudou de turma, desistiu.

**Minha sugestão: marcar como inativo, nunca apagar.**

**Por quê:** se apagar, as presenças dele nas aulas anteriores viram registros órfãos e o relatório daquelas aulas quebra. Presença é registro histórico — o passado não muda porque a pessoa saiu.

**O que quero de vocês:** só confirmação, se ninguém vir problema.

## 4. A chave do banco vai estar dentro do aparelho. O que ela pode fazer?

Este é o ponto mais importante da Parte 1.

Para o aparelho conversar com o banco, ele precisa de uma **chave de acesso** gravada dentro dele. E o nosso aparelho vai ficar **preso na parede de uma sala de aula, sozinho, sem ninguém olhando**. Quem tiver acesso físico e souber o que faz consegue tirar essa chave de lá. Então precisamos decidir de antemão o que ela consegue fazer.

**Minha sugestão: a chave só lê a lista da turma e só registra presença nova. Nunca apaga, nunca altera, nunca lê o histórico de outras aulas ou de outras turmas.**

| A chave poderia... | Sugestão |
|---|---|
| Ler a lista de alunos da turma daquela aula | ✅ sim — sem isso nada funciona |
| Ler a lista de alunos de todas as turmas | ❌ não |
| Registrar presença nova | ✅ sim — é o trabalho dela |
| Apagar uma presença já registrada | ❌ não |
| Alterar uma presença já registrada | ❌ não |
| Ler o histórico de aulas passadas | ❌ não |

**Por quê:** com essas regras, o pior que alguém consegue fazer com a chave roubada é bagunçar uma aula. Sem elas, consegue apagar o semestre inteiro. E isso é um bom parágrafo no relatório final: pensamos no aparelho como algo que pode ser violado, não como algo em ambiente seguro.

**O que quero de vocês:** alguém enxerga alguma função nossa que essas regras impedem sem querer?

## 5. Se o professor apertar Enviar duas vezes, a chamada duplica?

Cena real: ele aperta Enviar, a tela fica carregando, ele acha que não foi e aperta de novo. Ou a internet cai no meio e o aparelho tenta sozinho outra vez. Sem tratamento, a mesma aula entra duas vezes e todo mundo aparece com presença dobrada.

**Minha sugestão: resolver no banco.** Cada aula recebe um código único; se chegar uma aula com código que já existe, o banco recusa em vez de gravar de novo.

**Por quê:** o aparelho pode ser reiniciado no meio da aula, pode perder a memória, pode estar sendo usado por outro professor depois. O banco não. Quem defende de verdade é quem guarda.

**O que quero de vocês:** concordam, ou preferem travar o botão no aparelho também? (dá para ter os dois — o do banco é o que garante)

## 6. Na aula sem internet, quem descobre de quem é cada crachá?

Já está decidido que **a chamada acontece mesmo sem internet**: se o hotspot falhar, o aparelho não trava. Só que nesse caso ele não tem a lista da turma, então grava **apenas o número do crachá**, sem nome nem matrícula. Alguém precisa, depois, cruzar "crachá 4A2F91 → fulano".

**Minha sugestão: o banco faz esse cruzamento, na hora que o relatório chega.**

**Por quê:** o banco tem a lista completa de todo mundo; o aparelho só teria a de uma turma, e nem isso, já que foi justamente ela que não conseguiu baixar.

**O que quero de vocês:** ciência de uma consequência — **sem internet o aparelho aceita qualquer crachá encostado**, porque não tem como saber quem é da turma. A luz verde passa a significar "li o crachá", não "você é desta turma". Isso é honesto, vai declarado na documentação, e é o preço de a aula não parar. Alguém acha o preço alto demais?

---

# Parte 2 — A tela de histórico

## 7. Vale fazer uma tela na internet, além da tela do aparelho?

O professor exige, na entrega de **05/10**, um "dashboard ou interface de monitoramento". A tela que o **próprio aparelho já serve** — a que o professor abre no celular, com presentes, faltantes, botões e exportação — **já cumpre a exigência inteira**. Não falta nada para a nota.

Então a pergunta não é sobre nota: vale fazer uma **segunda** tela, hospedada na internet, mostrando o histórico de todas as aulas juntas?

| | O que é | Custo | O que ganhamos |
|---|---|---|---|
| **A** | Nada. Na apresentação mostramos a tela de administração que o Supabase já dá pronta | Zero | Quase nada — parece uma planilha |
| **B** | Uma página simples que lê o banco e desenha a tabela do histórico | Baixo. HTML e um pouco de JavaScript; não encosta no firmware | Quase todo o efeito da opção C |
| **C** | Dashboard completo, com gráficos, filtro por turma e por período | Alto | Pouco a mais que a B |

**Minha sugestão: a opção B — e só se alguém quiser pegar essa frente.**

**Por quê:** na apresentação de 27/10, abrir uma tela com oito aulas acumuladas e a frequência de cada aluno é o que transforma "um leitor de crachá" em "um sistema". A opção B compra quase todo esse efeito por uma fração do trabalho, e pode ser feita por quem não mexe em firmware — roda em paralelo com o resto, sem travar ninguém.

**O que quero de vocês:** a pergunta de verdade não é "fazemos ou não", é **quem quer pegar**. Se ninguém pegar, fica a opção A e está tudo certo: a exigência do professor já está cumprida pela tela do aparelho. Melhor não fazer do que fazer pela metade e chegar em 05/10 com uma tela quebrada.

---

# Parte 3 — A dúvida que o João levantou

## 8. Depois que o aparelho troca de rede, o professor volta para o painel ou para o login?

O João perguntou isso hoje, e é a melhor pergunta que apareceu até agora. Recapitulando o problema: o aparelho não consegue ser rede e usar internet ao mesmo tempo. Então, quando o professor faz login, o aparelho **derruba a própria rede**, vai buscar a lista da turma na internet e **volta a ser rede**. Nesse vai e volta, como ele sabe que deve devolver o painel do professor já logado, e não a tela de login de novo?

**A resposta:** o aparelho não precisa lembrar de nada. **Quem carrega a prova do login é o celular do professor.** Quando ele acerta a senha, o aparelho devolve a página junto com um **crachá digital** (um cookie), que fica guardado no navegador do celular. Daí em diante, toda vez que o celular fala com o aparelho, ele manda esse crachá junto, automaticamente. O aparelho não escolhe a tela por memória: ele olha se veio crachá válido. Com crachá, painel. Sem crachá, login.

A troca de rádio derruba a **conexão**, não a **sessão** — e é aí que a intuição engana. Some o cabo, não some o cadastro.

Só que isso impõe duas coisas, e as duas são sugestão minha:

**Minha sugestão 8a: o aparelho só derruba a própria rede depois que a resposta do login chegou inteira ao celular.** Se ele desligar o Wi-Fi no meio da resposta, o crachá nunca chega, e aí sim o professor cai no login de novo. É ordem de operação, não detalhe — e é a explicação mais provável se isso acontecer na bancada.

**Minha sugestão 8b: testar isso num iPhone e num Android antes de escrever o resto do firmware.** Quando o celular entra numa rede sem internet, o sistema abre sozinho aquela janelinha de "entrar na rede" — que não é o Safari nem o Chrome, é um mini-navegador separado, com memória própria. Dois riscos moram aí: o crachá pode ficar preso nessa janelinha (que o iPhone fecha sozinho), e o celular, vendo rede sem internet, pode pular para o 4G sozinho e levar o painel do professor junto, no meio da aula.

Os dois têm solução conhecida, e a mesma família de solução: depois do login, o aparelho responde ao teste de internet do celular de um jeito que o sistema entenda "estou dentro, pode parar de perguntar", e o professor segue no navegador normal. Se na bancada isso se mostrar instável, a alternativa é manter o professor dentro da própria janelinha do sistema, em vez de mandá-lo para o navegador.

⚠️ **Corrigido em 11/09.** Este ponto tinha uma terceira saída, mais bruta: carregar a credencial no próprio endereço da página, para nenhuma janela fechada importar. **Ela caiu**, por observação do Cauã aceita no tópico 3 em 10/09: endereço com credencial dentro fica guardado no histórico do celular, aparece em print de tela e sobra no registro de quem acessou. Está na especificação como R25, e não volta à mesa.

**O que quero de vocês:** isso não se decide na teoria — muda entre iPhone e Android e entre versões de sistema. E dá para testar **já**: precisa só do ESP32, que a gente já tem. Nada de leitor de crachá, nada de LED. Subir a rede, fazer login, trocar o rádio, voltar e continuar logado.

Vale como primeiro pedaço de código do projeto por três motivos: é o osso mais duro que a gente tem, é o que o professor pede como *"código inicial preparado"* no checklist de 21/09, e se funcionar, o resto do firmware é montagem.

**Quem quer pegar esse teste?**

---

# Parte 4 — Mensagem pronta para o João

Duas pendências não são decisão: estão paradas esperando informação que só ele tem. Mensagem pronta para copiar e colar — a formatação com asterisco simples é a do WhatsApp.

```
Fala João! Preciso de duas coisas tuas pra gente não atrasar o cronograma 👇

*1) O que a gente já tem na mão, fisicamente?*

Quero fechar a lista de compras, mas não sei o que já está com quem. Pelo que tenho anotado, já temos: o ESP32, o leitor de crachá RC522, as tags e a protoboard.

E faltaria comprar:
• LED RGB (catodo comum) + 3 resistores de 220 ohm
• Buzzer ativo 5V
• Jumpers macho-macho e macho-fêmea
• Fonte 5V 2A (ou cabo micro-USB com carregador)

Consegue confirmar o que realmente está na mão, e com quem está?

⚠️ Detalhe importante: se o buzzer for de 5V, ele não liga direto no ESP32 — precisa de um transistor 2N2222 e um resistor de 1k ohm junto. Vale comprar os dois na mesma ida, sai mais barato que voltar na loja depois.

Sugestão: loja física, uma pessoa só comprando tudo de uma vez. É tudo peça de R$5 a R$20, e loja online demora de 5 a 10 dias — risco à toa. *As peças precisam estar na mão até 12 ou 14/09*, porque no dia 22/09 a entrega é o protótipo eletrônico já funcionando.

Quem compra?

*2) A impressora 3D*

Você comentou de imprimir a caixa. Antes da gente contar com isso, preciso saber:
• Você tem acesso de verdade a uma impressora? (sua, de amigo, laboratório da UVA, ou serviço pago?)
• Alguém de nós sabe modelar em CAD? Sem isso, ter a impressora não resolve.
• Do modelo pronto até a peça na mão, quanto tempo leva? (contando fila e uma reimpressão, se falhar)

A entrega da estrutura física é *29/09*. Se a resposta for "talvez", meu voto é irmos de MDF cortado em papelaria com parafuso: barato, rápido, o professor aceita e não depende de ninguém aprender CAD em três semanas. Mas se a impressora for garantida, beleza demais.

O que a caixa precisa: ficar presa na parede, ter passagem pro cabo da fonte, e deixar o leitor de crachá acessível pela frente, pra encostar a tag.

Ah, e aquela tua dúvida de hoje — como o aparelho sabe voltar pro painel do professor em vez da tela de login depois de trocar de rede — virou um tópico no repositório, com a resposta: github.com/caioplaninschek/presente/issues/3

Pergunta muito boa, acabou virando o primeiro pedaço de código do projeto.
```

---

# As oito sugestões em uma tela

| # | Assunto | Sugestão |
|---|---|---|
| 1 | Crachás por aluno | Um ativo por vez; perdeu, troca o número |
| 2 | Aluno em várias turmas | Sim — aluno cadastrado uma vez, com uma lista à parte de quem está em qual turma |
| 3 | Aluno que sai da turma | Marcar inativo, nunca apagar |
| 4 | O que a chave do banco pode fazer | Só ler a lista da turma e registrar presença nova. Nunca apagar, alterar nem ler o passado |
| 5 | Enviar apertado duas vezes | Cada aula com código único; o banco recusa a repetida |
| 6 | Aula sem internet | O banco cruza crachá → aluno na chegada do relatório |
| 7 | Tela de histórico na internet | Fazer a versão simples — **se** alguém pegar. Sem dono, não fazemos |
| 8 | Voltar ao painel depois da troca de rede | O crachá digital resolve; responder o login antes de derrubar a rede, e testar em iPhone e Android já |

**O que precisa sair da conversa:** concordância ou objeção nos pontos 1 a 6, um dono para o 7 (ou a decisão de não fazer) e um dono para o teste do 8 — que é o mais urgente dos três, porque vira o código inicial que o professor cobra em 21/09.
