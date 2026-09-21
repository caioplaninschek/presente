# Mapa: Presente! até 30/11

Label: `wayfinder:map`
Criado: 2026-09-06

## Destination

**30/11/2026 — o Presente! entregue e funcionando:** protótipo físico montado, firmware, portal do professor, API na nuvem, mais a documentação completa — tendo passado por todos os 8 marcos do professor (07/09 · 14/09 · 22/09 · 29/09 · 05/10 · 20/10 · 27/10 · 30/11).

O mapa fecha quando não sobrar nada a **decidir** antes de alguém ir construir. O roteiro de execução não sai daqui: sai do `/to-spec` + `/to-tickets` depois.

> Escopo longo assumido conscientemente. O Caio foi avisado do risco de sprawl (tickets que se invalidam antes de serem trabalhados) e reafirmou o destino em 30/11. Mitigação: só vira ticket o que dá para formular com precisão hoje; outubro e novembro ficam em **Not yet specified** e graduam quando a frente chegar lá.

## Notes

- **Fonte da verdade das decisões de produto:** `docs/spec.md`. Quantas rodadas estão travadas e até qual decisão, o cabeçalho `Status:` da spec diz. Ler antes de propor qualquer mudança de arquitetura, contrato JSON ou pinagem.
- **Plan, don't do vale o default.** Nenhum ticket deste mapa escreve firmware, portal ou documento entregável. Ticket que ler "construir o X" está mal-tipado.
- **Skills por sessão:** `grilling` + `domain-modeling`. Tickets de pesquisa vão para subagente.
- **Idioma:** português brasileiro, exceto código.
- **O Caio é o PO, não o engenheiro do time.** Carregar decisões técnicas com recomendação clara e explicação em prosa antes de qualquer seletor; perguntar só o que é dele (produto, time, logística).
- **Este repo é exceção à lei anti-vazamento** (arquivos de IA podem ser commitados às claras), mas **assinatura de IA em commit continua proibida**: sem `Co-Authored-By`, sem `🤖 Generated with`.
- **Dois lugares, uma fonte da verdade.** As decisoes que precisam do grupo sao debatidas nas **Issues do GitHub** (o forum da equipe, em linguagem de leigo, alimentado por docs/pauta-do-grupo.md); estes tickets em .scratch continuam sendo o registro de trabalho. O fluxo e de mao unica: decisao fechada no issue -> escrita na spec.md -> ticket local fechado. Nunca o contrario.
- **Calendário manda.** Todo ticket que tem data de entrega atrás dele é urgente por construção, não por importância.
- **A execução saiu daqui em 11/09.** A quebra em epics por entrega, os vértices e o grafo de dependência vivem em `.scratch/prototipo/graph.md`, e as issues de atribuição estão no GitHub, da #5 em diante. Este mapa continua dono só dos fios de decisão que sobraram: o veredito do ticket 09 e o ticket 08, reaberto pelo R19. A metade de RLS do ticket 06 fechou em 13/09, na #21 (R33–R35).

## Decisions so far

<!-- uma linha por ticket fechado -->

- [Divisão de responsabilidades confirmada com o grupo](issues/01-divisao-de-responsabilidades.md): o ticket estava mal formulado — a divisão é **artefato de documentação, não contrato operacional**. Travada sem consulta formal; o papel do Caio ("Integração e documentação") é amplo por desenho, para que fazer o trabalho de outro não contradiga o papel.
- [Quem escreve firmware, e como o time evita conflito no repo](issues/05-quem-escreve-firmware.md): direto na `main`, sem branch nem PR — o número real de mãos no C++ é 1 ou 2, e a modularização já decidida (R9) faz o trabalho que o processo faria. Até 22/09, entre programas de bancada, esse papel é de cada um ter a sua pasta e o seu ambiente (R28). ⚠️ **Uma exceção entrou em 15/09 (R39):** quem tomou por conta própria uma decisão que o grupo ainda não viu abre pull request, para ela poder cair antes de entrar na `main` — foi o caso do portal do professor (#29). Revisão obrigatória continua fora.
- [Comportamento do rádio: quando sincronizar e o que acontece quando a troca falha](issues/08-comportamento-do-radio.md): sincronizar **no login**, não no Iniciar; 2 tentativas e depois `ESP.restart()`; `sessao_atual.json` vira requisito para a sessão sobreviver ao reboot. A alternativa de passar a internet para o celular do professor foi descartada por razão acadêmica, não técnica.
- [Caminho viável do ESP32 até o Supabase](issues/03-esp32-https-supabase.md): o caminho existe — `HTTPClient` + `WiFiClientSecure` + `setInsecure()`, sem lib de Supabase, sem NTP, sem certificado pinado (a Supabase troca de CA sozinha). Fechar o AP antes do TLS virou requisito de memória, não só de canal. A transição de rádio leva **5–15 s**, não 3, e é o ponto de falha mais provável do projeto.
- [Lista final de compras](issues/02-lista-final-de-compras.md): fechada pela issue #4 (R22) — o João comprou tudo e as peças estão com ele. Nenhuma frente do projeto depende de compra. O transistor do buzzer só se compra depois da checagem de bancada (módulo de 3 pinos já tem driver; peça de 2 pinos pode bastar em 3,3 V).
- [O dashboard em nuvem existe, ou o portal do ESP32 basta?](issues/07-dashboard-em-nuvem-escopo.md): existe, opção B (página simples lendo o banco, sem tocar no firmware), e o dono é o Igor — issue #2 (R21). Fecha `HU-30`.
- **Esquema do banco** (metade do ticket [06](issues/06-esquema-supabase-e-rls.md)): `alunos`, `professores`, `turmas`, `alunos_turmas`, `sessoes` (com `estado`) e `eventos` — issue #1 (R20), com as três entidades propostas pelo Cauã. ~~O `estado` da sessão é o que implementa a idempotência do Enviar~~ — quem a implementa, desde a **R33**, é a chave primária `sessaoId`: se o id já existe, a função responde `ja_enviado` e não grava nada (`on conflict do nothing`). O `HU-39` continua fechado, e o aparelho só escreve `finalizada` (**R42**, 16/09). A outra metade fechou em 13/09, na linha seguinte.
- **Quem acessa o banco, e como** (outra metade do ticket [06](issues/06-esquema-supabase-e-rls.md)): o aparelho não toca tabela nenhuma e só entra por duas funções do próprio banco, `roster` e `enviar_relatorio`; o histórico só se lê com login; a chave é a publicável — issue #21 (R33–R35). Tentar apagar ou alterar com a chave do aparelho dá erro de permissão.

- **Credencial fora do endereço da página** (item 2 do ticket [09](issues/09-retorno-ao-painel-apos-troca-de-radio.md)): a opção (c) — carregar o token na própria URL — **caiu**, por objeção do Cauã aceita na issue #3 em 10/09 e escrita na spec como **R25** em 11/09. Restam (a) manter tudo no CNA e (b) trazer o professor pelo CNA e seguir em `192.168.4.1`. ⚠️ O **veredito do ticket segue aberto** — o que fechou foi uma das três saídas, não o ticket.

⚠️ **Decisão revertida em 10/09:** o item "AP e STA nunca simultâneos" (R4) caiu. O João mostrou na issue #3 que `WIFI_AP_STA` é nativo, e a documentação da Espressif confirma: a restrição é de **canal** (as duas interfaces vão para o canal do STA, com CSA avisando as estações), não de impossibilidade. O impedimento que resta é de **memória** (40–50 KB do handshake TLS). O projeto passa a ter **Plano A** (simultâneo) e **Plano B** (alternância), decididos por medição na bancada — ticket [09](issues/09-retorno-ao-painel-apos-troca-de-radio.md), testes 7c e 7d do §8 da spec. Por consequência, o ticket [08](issues/08-comportamento-do-radio.md) voltou a `reopened`.

## Not yet specified

Névoa em escopo, ainda sem nitidez para virar ticket:

- **Apresentação da Semana da Computação (27/10).** Quem fala, o que é demonstrado ao vivo, e qual o plano B se a demo falhar na hora. Só dá para decidir com o protótipo de pé.
- **Vídeo de 2 minutos (20/10).** Roteiro, quem grava, quem edita, o que aparece. Depende do protótipo estar montado.
- **Relatório técnico ABNT (30/11).** Ferramenta, estrutura, quem consolida, e como as decisões da spec viram texto acadêmico. Depende de quase tudo.
- ~~**Dados de teste.** Quantos alunos fictícios, com que nomes, e como as tags físicas são atribuídas a eles. Fica nítido quando o Supabase existir.~~ Graduou em 11/09: os dados entram no banco pela #21, e os UIDs das tags reais saem da leitura na #25 (13/09).
- **Design e usabilidade do portal do professor.** Provável ticket de `prototype` quando o firmware servir a primeira página.
- **Instalação na sala para a demo.** Tomada disponível, fixação, autorização da UVA para prender algo na parede. Pode virar `task` perto de 27/10.

## Out of scope

Fora do destino deste mapa. Fechado, não gradua.

- **Integração real com o sistema acadêmico da UVA.** Sem acesso; o Supabase encena esse papel no MVP.
- **Celular do aluno como tag (NFC no aparelho pessoal).** Citado na documentação como evolução, não construído.
- **Biometria e catraca.** Descartado por custo, LGPD (dado sensível) e escopo.
- **Controle de saída da sala.** Decidido em R8: duplica estado e falha sem resolver o problema.
- **Backend próprio auto-hospedado (Node/Python + PostgreSQL).** Substituído pelo Supabase.
