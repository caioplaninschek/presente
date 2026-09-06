# Diário de bordo — Presente!

Registro semanal da evolução do projeto. Atualizado antes de cada entrega de terça.
O histórico de commits deste repositório é a prova primária; este arquivo é a versão legível.

---

## Semana 1 — 27/08/2026 · Entrega 01

**Entregue:** PDF de identificação da equipe, tema, introdução e objetivo.

Formação dos 5 integrantes e escolha do tema. A proposta do professor era resolver um problema real do campus da Barra; a partir de um comentário dele em aula, o grupo escolheu atacar o tempo gasto com chamada manual.

---

## Semana 2 — 02/09/2026 · Entrega 02

**Entregue:** PDF com Canvas do projeto, lista de componentes e diagrama em blocos.

Primeira formulação técnica: ESP32 + RC522 + LED + buzzer, com portal cativo Wi-Fi para o professor. O projeto se chamava **SmartPresence** nesta fase.

Duas coisas desta entrega foram revistas depois, e a revisão está registrada na spec: o controle de entrada/saída por dois toques (retirado) e o RTC como segundo sensor (substituído).

---

## Semana 3 — 06/09/2026 · preparação da Entrega 03

**Foco:** fechar a especificação antes de escrever qualquer linha de código.

Cinco rodadas de decisão sobre o produto, todas registradas em [`docs/spec.md`](docs/spec.md):

- **Nome.** SmartPresence → **Presente!**
- **Arquitetura de dados.** A lista da turma sai do aparelho e passa a vir da API. O aparelho deixa de guardar cadastro de aluno — só a turma da sessão corrente, apagada no encerramento.
- **Sincronização.** Descartada a chamada de API por aluno (40 idas à internet durante a aula). Passou para **dois momentos**: uma no login e uma no envio. Entre elas, tudo local e abaixo de 200 ms.
- **Segundo sensor: entrou e saiu na mesma semana.** RTC DS3231 descartado — era redundante, porque o navegador do professor já entrega a hora. No lugar entrou o **SW-420**, para detectar violação do aparelho, que fica fixo e sem vigilância numa sala de aula. Na manhã do dia 06, porém, o João confirmou com o professor que este projeto está dispensado da exigência de dois sensores, e o SW-420 saiu junto: ele não participa da chamada, e mantê-lo custaria compra, calibração de limiar no lugar definitivo e um estado a mais no firmware. O projeto fica com um sensor, o **RC522**.
- **Ferramentas.** PlatformIO no lugar do Arduino IDE; portal servido do LittleFS; credenciais fora do git.

**Pesquisa técnica concluída:** validado que o ESP32 consegue falar com o Supabase por HTTPS (`setInsecure()`, sem certificado pinado — a Supabase troca de autoridade certificadora sem aviso). Descoberto que a alternância entre modo hotspot e modo cliente leva **5 a 15 segundos**, não os 3 estimados, e que é o ponto de falha mais provável do projeto. O tratamento entrou na spec: sincronizar durante o login, duas tentativas de recuperação e reinício automático, com a sessão persistida em disco para sobreviver ao reboot.

**Infraestrutura:** repositório criado e publicado; mapa de decisões do projeto em `.scratch/presente/`.

**Entrega 03 fechada.** O documento do projeto detalhado ficou pronto em quatro páginas, dentro do limite pedido, cobrindo as oito seções do enunciado e a pergunta obrigatória sobre o principal desafio técnico. Duas divergências em relação às entregas anteriores foram declaradas no próprio texto, em vez de ficarem por conta do leitor: a troca do nome do projeto e a retirada do registro de saída por segundo toque, que o Canvas da Entrega 02 prometia. O diagrama em blocos foi redesenhado — o da entrega anterior mostrava o aparelho falando com o celular do professor e com a nuvem ao mesmo tempo, coisa que o rádio do ESP32 não faz. Na manhã do dia 06 ele foi refeito no **draw.io**, com o arquivo editável versionado em `docs/diagrama-blocos.drawio`: o diagrama deixa de ser desenho à mão em SVG e passa a ter fonte que qualquer integrante abre e ajusta. O SW-420 saiu do desenho junto com o resto do projeto.

Falta preencher o nome da turma, campo que ficou em branco no PDF.

**Próximo:** Entrega 04 (14/09) — programação planejada, lógica, conexões e plano de testes.
