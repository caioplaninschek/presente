# Mapa: Presente! até 30/11

Label: `wayfinder:map`
Criado: 2026-09-06

## Destination

**30/11/2026 — o Presente! entregue e funcionando:** protótipo físico montado, firmware, portal do professor, API na nuvem, mais a documentação completa — tendo passado por todos os 8 marcos do professor (07/09 · 14/09 · 22/09 · 29/09 · 05/10 · 20/10 · 27/10 · 30/11).

O mapa fecha quando não sobrar nada a **decidir** antes de alguém ir construir. O roteiro de execução não sai daqui: sai do `/to-spec` + `/to-tickets` depois.

> Escopo longo assumido conscientemente. O Caio foi avisado do risco de sprawl (tickets que se invalidam antes de serem trabalhados) e reafirmou o destino em 30/11. Mitigação: só vira ticket o que dá para formular com precisão hoje; outubro e novembro ficam em **Not yet specified** e graduam quando a frente chegar lá.

## Notes

- **Fonte da verdade das decisões de produto:** `docs/spec.md`. As rodadas Q1–Q9 (05/09), R1–R8 (05/09) e R9–R12 (06/09) já estão travadas lá. Ler antes de propor qualquer mudança de arquitetura, contrato JSON ou pinagem.
- **Plan, don't do vale o default.** Nenhum ticket deste mapa escreve firmware, portal ou documento entregável. Ticket que ler "construir o X" está mal-tipado.
- **Skills por sessão:** `grilling` + `domain-modeling`. Tickets de pesquisa vão para subagente.
- **Idioma:** português brasileiro, exceto código.
- **O Caio é o PO, não o engenheiro do time.** Carregar decisões técnicas com recomendação clara e explicação em prosa antes de qualquer seletor; perguntar só o que é dele (produto, time, logística).
- **Este repo é exceção à lei anti-vazamento** (arquivos de IA podem ser commitados às claras), mas **assinatura de IA em commit continua proibida**: sem `Co-Authored-By`, sem `🤖 Generated with`.
- **Calendário manda.** Todo ticket que tem data de entrega atrás dele é urgente por construção, não por importância.

## Decisions so far

<!-- uma linha por ticket fechado -->

- [Caminho viável do ESP32 até o Supabase](issues/03-esp32-https-supabase.md): o caminho existe — `HTTPClient` + `WiFiClientSecure` + `setInsecure()`, sem lib de Supabase, sem NTP, sem certificado pinado (a Supabase troca de CA sozinha). Fechar o AP antes do TLS virou requisito de memória, não só de canal. A transição de rádio leva **5–15 s**, não 3, e é o ponto de falha mais provável do projeto.

## Not yet specified

Névoa em escopo, ainda sem nitidez para virar ticket:

- **Apresentação da Semana da Computação (27/10).** Quem fala, o que é demonstrado ao vivo, e qual o plano B se a demo falhar na hora. Só dá para decidir com o protótipo de pé.
- **Vídeo de 2 minutos (20/10).** Roteiro, quem grava, quem edita, o que aparece. Depende do protótipo estar montado.
- **Relatório técnico ABNT (30/11).** Ferramenta, estrutura, quem consolida, e como as decisões da spec viram texto acadêmico. Depende de quase tudo.
- **Dados de teste.** Quantos alunos fictícios, com que nomes, e como as tags físicas são atribuídas a eles. Fica nítido quando o Supabase existir.
- **Design e usabilidade do portal do professor.** Provável ticket de `prototype` quando o firmware servir a primeira página.
- **Calibração do SW-420 no lugar definitivo.** Limiar, debounce, e falso-positivo com porta batendo. Só decidível com a caixa montada e fixada.
- **Instalação na sala para a demo.** Tomada disponível, fixação, autorização da UVA para prender algo na parede. Pode virar `task` perto de 27/10.

## Out of scope

Fora do destino deste mapa. Fechado, não gradua.

- **Integração real com o sistema acadêmico da UVA.** Sem acesso; o Supabase encena esse papel no MVP.
- **Celular do aluno como tag (NFC no aparelho pessoal).** Citado na documentação como evolução, não construído.
- **Biometria e catraca.** Descartado por custo, LGPD (dado sensível) e escopo.
- **Controle de saída da sala.** Decidido em R8: duplica estado e falha sem resolver o problema.
- **Backend próprio auto-hospedado (Node/Python + PostgreSQL).** Substituído pelo Supabase.
