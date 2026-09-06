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
- **Sensor 2.** RTC DS3231 descartado — era redundante, porque o navegador do professor já entrega a hora. No lugar entrou o **SW-420**, detectando violação do aparelho, que fica fixo e sem vigilância numa sala de aula.
- **Ferramentas.** PlatformIO no lugar do Arduino IDE; portal servido do LittleFS; credenciais fora do git.

**Pesquisa técnica concluída:** validado que o ESP32 consegue falar com o Supabase por HTTPS (`setInsecure()`, sem certificado pinado — a Supabase troca de autoridade certificadora sem aviso). Descoberto que a alternância entre modo hotspot e modo cliente leva **5 a 15 segundos**, não os 3 estimados, e que é o ponto de falha mais provável do projeto. O tratamento entrou na spec: sincronizar durante o login, duas tentativas de recuperação e reinício automático, com a sessão persistida em disco para sobreviver ao reboot.

**Infraestrutura:** repositório criado e publicado; mapa de decisões do projeto em `.scratch/presente/`.

**Próximo:** Entrega 03 (07/09, 20:00) — projeto detalhado em PDF de 2 a 4 páginas.
