# Relatório técnico da entrega final: índice

O relatório técnico é o documento complementar da entrega final (R85). O enunciado, trazido pelo Caio em 25/11, pede um PDF simplificado, com a identificação da equipe, a foto da exposição e o link do código, e esse PDF cita o link deste relatório. O enunciado não trouxe modelo, e o relatório segue o das Entregas 05 e 06, na ABNT.

O Gabriel montou o relatório em 02/11, a partir dos capítulos que o grupo escreveu nesta pasta, e o Caio o revisou e concluiu em 25/11, com as correções da revisão: a exposição de 27/10, o vídeo do Cauã, a contagem dos toques acima de 200 ms, as correções que os capítulos já tinham recebido e as referências. Cada capítulo mora num arquivo, já com a numeração final das seções, dos quadros e das figuras, e o PDF sai deles para `docs/entregas/entrega-final/` (tarefa do PDF do relatório, #72).

## Os arquivos, na ordem do documento

| Parte | Arquivo | Quem escreveu |
|---|---|---|
| Capa, resumo e palavras-chave | [`resumo.md`](resumo.md) | Gabriel, revisado pelo Caio |
| 1 Introdução e arquitetura | [`introducao-e-arquitetura.md`](introducao-e-arquitetura.md) (#66) | Gabriel |
| 2 O firmware | [`firmware.md`](firmware.md) (#67) | Cauã, condensado pelo Gabriel |
| 3 A estrutura física | [`estrutura-fisica.md`](estrutura-fisica.md) (#68) | Gabriel |
| 4 O portal do professor, a página de histórico e o dashboard | [`portal-historico-e-dashboard.md`](portal-historico-e-dashboard.md) (entrega do dashboard, #52) | Igor, condensado pelo Gabriel |
| 5 Os testes e os resultados | [`testes-e-resultados.md`](testes-e-resultados.md) (#69) | Gabriel |
| 6 Limitações e caminhos descartados | [`limitacoes-e-descartes.md`](limitacoes-e-descartes.md) (#70) | Cauã, condensado pelo Gabriel |
| 7 Considerações finais, quem fez cada parte (Quadro 11) e referências | [`consideracoes-finais-e-referencias.md`](consideracoes-finais-e-referencias.md) (#71) | Gabriel, revisado pelo Caio |

O sumário se gera na montagem do PDF. As versões longas do firmware, do dashboard e das limitações, anteriores à consolidação, continuam no histórico do git.

## Regras para quem corrigir

- Siga o `docs/padrao-editorial.md`: primeira pessoa do plural ("decidimos"), sem travessão, e número de teste, de pacote ou de issue com o nome por extenso ("a busca da turma (#43)"). O código de decisão da spec (R74, por exemplo) não aparece no texto.
- Fato que não está na `docs/spec.md` nem veio do Caio não entra. O que veio só do relato de alguém entra como relato.
- Medição e resultado de teste moram só nos testes (capítulo 5), e limitação se analisa só nas limitações (capítulo 6).
- Quadro ou figura novos renumeram os seguintes, e as chamadas no texto acompanham.
