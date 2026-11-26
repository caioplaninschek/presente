# 1 INTRODUÇÃO E ARQUITETURA

O Presente! é um terminal embarcado que registra a presença dos alunos em sala de aula pela leitura do crachá, desenvolvido na disciplina de Sistemas Embarcados da UVA Barra, na turma 4172CMPN6A_P1. O problema de partida é a chamada manual: o professor lê a lista nome por nome no site da instituição e marca as faltas numa tabela, o que consome de 10 a 15 minutos de cada aula, produz o erro de marcar a falta no aluno errado e abre espaço para alguém responder pelo colega ausente.

O objetivo do protótipo é que o aluno registre a presença encostando o crachá num terminal fixo ao lado da porta e receba na hora um sinal de luz e de som. O professor opera o aparelho pelo navegador do celular, na rede Wi-Fi que o próprio aparelho põe no ar, sem instalar aplicativo, e, no fim da aula, envia o relatório da chamada para um banco na nuvem; se o envio falha, ele pode baixar o relatório no celular, pelo botão Compartilhar. O tamanho do protótipo seguiu um critério que o Caio fixou em 23/09, o núcleo completo com os cortes declarados, e a seção das limitações reúne os cortes, com a razão de cada um.

## 1.1 Os blocos e o caminho de uma aula

Cada toque de crachá se resolve inteiro no circuito do aparelho, sem rede. A rede entra por dois motivos: o professor opera o aparelho pelo celular, e a lista da turma vem da nuvem, para onde também vai o relatório (Quadro 1).

**Quadro 1: Os blocos da solução**

| Bloco | O que faz |
|----|----|
| Crachá | Leva o UID, o número de série do chip, que o cadastro do banco liga ao aluno; o aparelho só o lê |
| Leitor RC522 | Lê o UID do crachá encostado e o passa ao ESP32 pelo barramento SPI |
| ESP32 DevKit V1 | Decide cada toque, grava a chamada na memória flash, põe no ar a rede do aparelho, serve o portal do professor e fala com o banco |
| LED RGB e buzzer | Mostram ao aluno o resultado do toque, pela cor e pelo som |
| Celular do professor | Abre o portal do professor, as páginas que o ESP32 serve, sem aplicativo |
| Hotspot de celular | Dá internet ao aparelho no login e no envio |
| Banco no Supabase | Guarda os alunos, com o crachá de cada um, as turmas, os professores e as chamadas enviadas |
| Página de histórico | Mostra a quem tem login as aulas registradas no banco |

Fonte: elaborado pelos autores (2026).

Quando o professor conecta o celular na rede do aparelho, a página de login abre sozinha, pelo portal cativo, e ele entra com usuário e senha, que o ESP32 confere na própria memória, para o login funcionar sem internet. Aceita a senha, o aparelho entra como cliente no hotspot, busca a lista da turma no banco, guarda essa lista e sai do hotspot, sem tirar a própria rede do ar. Se a busca falha, o aparelho entra no modo offline, em que aceita todo crachá lido e grava só o UID, e o relatório se completa no banco, no envio.

Entre o Iniciar e o Enviar, o aparelho não usa a internet. O aluno encosta o crachá, o ESP32 confere o UID na lista da turma, quando a tem, grava o evento na memória da placa e dá o sinal, e cada aluno tem uma presença só por chamada. O professor acompanha a contagem pelo celular e pode lançar à mão a presença de quem esqueceu o crachá. No fim da aula, o "Enviar e fechar a chamada" manda o relatório ao banco, e o Encerrar apaga do aparelho a lista e os eventos. A página de histórico, publicada pelo GitHub Pages, lê o banco, e ela e o aparelho nunca se falam direto.

## 1.2 As peças e as ligações

O circuito tem um sensor, o leitor RC522, e dois atuadores, o LED e o buzzer; o sensor é um só porque o professor dispensou este projeto da exigência de dois. O João comprou as peças, que o Quadro 2 lista com a ligação de cada uma ao ESP32.

**Quadro 2: As peças e as ligações ao ESP32**

| Peça | O que faz | Ligação ao ESP32 |
|----|----|----|
| ESP32 DevKit V1 / 32D | Controlador, com Wi-Fi e memória flash | Conector USB-C para a alimentação e a gravação |
| Leitor RFID/NFC RC522, de 13,56 MHz | Sensor: lê o UID do crachá | SDA no GPIO 5, SCK no GPIO 18, MOSI no GPIO 23, MISO no GPIO 19 e RST no GPIO 22, com 3,3 V |
| LED RGB de catodo comum | Atuador 1: vermelho, verde ou azul | GPIO 25, GPIO 26 e GPIO 27, cada um com resistor de 300 Ω |
| Buzzer ativo de 5 V | Atuador 2: o bip da presença e o buzz da recusa | GPIO 33, pela base de um transistor TIP122, que chaveia os 5 V do VIN |
| Cartões com UID de 4 bytes | Crachás dos testes e da entrega final | Lidos pelo RC522, por aproximação |

Fonte: elaborado pelos autores (2026).

O leitor recebe os 3,3 V do regulador da placa, porque o RC522 trabalha em 3,3 V (NXP Semiconductors, 2016), e o buzzer, que é de 5 V, é chaveado por um transistor, porque o pino do ESP32 entrega 3,3 V e pouca corrente. O aparelho não tem relógio próprio: a hora vem do navegador do professor, no login e no Iniciar.

## 1.3 O rádio

O principal desafio técnico do projeto é fazer o rádio único do ESP32 atender o professor e a nuvem sem derrubar o portal. O módulo tem uma antena só, na faixa de 2,4 GHz, e 520 KB de SRAM (Espressif Systems, 2026). Até 10/09, entendíamos que manter a rede do aparelho e a conexão ao hotspot ao mesmo tempo derrubaria o celular do professor. Nesse dia, o João contestou essa premissa, e o Caio confirmou na documentação da Espressif que o modo simultâneo, o `WIFI_AP_STA`, é nativo do ESP32: as duas interfaces ficam no canal do hotspot, e a rede do aparelho avisa a troca aos celulares conectados antes de mudar (Espressif Systems, [2022]).

Em 21 e 22/09, os testes na placa mostraram memória de sobra na conexão segura, com registro nas issues, e, pelo relato do grupo, sem log nem foto, o celular do professor no painel quando o aparelho entrou no hotspot, em iPhone e em Android, e o veredito do rádio (#19) adotou o modo simultâneo, no lugar da alternância, que custaria de 5 a 15 s a cada troca. O aparelho só entra no hotspot logo depois do login e no Enviar, e sai dele no fim de cada ida, para não arrastar a própria rede de canal em canal se o hotspot sumir. Concentrar a rede nesses dois instantes, em vez de consultar o banco a cada crachá, faz as idas à internet de uma turma de 40 alunos caírem de cerca de 40 para 2. A internet vem do hotspot de um celular porque a rede da UVA pede login por portal cativo ou por autenticação corporativa, que o ESP32 não atravessa.

## 1.4 O banco na nuvem

O banco fica no plano gratuito do Supabase, que hospeda bancos PostgreSQL e os expõe na internet sem que precisemos escrever servidor; o trabalho, do Caio, foi criar as tabelas, os privilégios e duas funções dentro do banco. Como não temos acesso ao sistema acadêmico da UVA, o banco faz esse papel no protótipo: dele vem a lista da turma, e para ele vai o relatório.

O aparelho só entra no banco por essas duas funções. No login, `roster` devolve a matrícula, o nome e o UID dos alunos ativos da turma, e, no Enviar, `enviar_relatorio` grava a chamada e os eventos numa transação só. Se a mesma chamada chega de novo, o banco responde que já a tinha, sem gravar nada, e por isso o professor pode repetir o envio sem duplicar o relatório. O aparelho e a página de histórico usam a chave publicável do Supabase: sem login, ela só executa as duas funções, e, com o e-mail e a senha de uma conta criada à mão, a página lê as tabelas, sem gravar nada.
