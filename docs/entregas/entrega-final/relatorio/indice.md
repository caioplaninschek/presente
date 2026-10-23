# Relatório técnico da entrega final: índice

O relatório técnico é uma das peças da entrega final (R71) e, pelo entendimento do Caio, é para a semana seguinte à exposição de 27/10 (R81, 23/10). É um documento só, na ABNT, no modelo das Entregas 05 e 06 até o professor passar o dele, com o capítulo do portal do professor, da página de histórico e do dashboard dentro dele (R80). Cada seção mora num arquivo desta pasta, para cada integrante escrever a sua sem escrever por cima de outro (esqueleto do relatório, #65).

## O que ainda não sabemos

Em 20/10, o professor disse que o relatório terá um modelo, que ele vai passar, e não respondeu sobre o número de páginas (pergunta 2 da entrega final em `docs/entregas.md`). Até o modelo chegar, **nenhuma seção tem extensão definida**, e o campo "Extensão" de cada arquivo fica em branco. Estimativa nossa não entra no lugar da resposta: o `docs/padrao-editorial.md` manda perguntar em vez de preencher, e um número escrito aqui viraria uma regra que alguém cumpriria.

Quando o modelo do professor chegar, este índice passa a seguir o dele, e as seções se reagrupam sem reescrever o texto. O modelo entra aqui, com a data.

## O modelo, das Entregas 05 e 06, até chegar o do professor

- **Capa** como a da Entrega 06 (`docs/entregas/entrega-06/entrega-06.md`): o logotipo da UVA, o curso, os cinco nomes com a matrícula, o título do projeto e a linha da entrega, a disciplina, o professor, a cidade e o ano.
- **Resumo** com palavras-chave.
- **Sumário.**
- **Seções numeradas, com o título em caixa alta.** A numeração se dá na consolidação (#71), e os arquivos desta pasta não numeram.
- **Figuras e quadros** com a legenda acima e "Fonte: elaborado pelos autores (2026)." abaixo, quando o material é do grupo.
- **Referências** na ABNT.

O resumo, as palavras-chave, as considerações finais, as referências e o quadro de quem fez o quê não têm arquivo aqui: ficam com a consolidação (#71).

## As seções, na ordem do documento

A ordem abaixo é uma proposta do esqueleto, e a consolidação (#71) pode mudá-la.

| Seção | Fonte | Quem pegou |
|---|---|---|
| Introdução e arquitetura, em [`introducao-e-arquitetura.md`](introducao-e-arquitetura.md) (#66) | §1, §3, §4, §4.1, §7 e §10 da `docs/spec.md`; R33 a R35 e R45 | ninguém |
| O firmware, em [`firmware.md`](firmware.md) (#67) | §5 da `docs/spec.md`; R51, R66 a R68 e R74 a R79 | Cauã |
| A estrutura física, em [`estrutura-fisica.md`](estrutura-fisica.md) (#68) | R53 a R64 da `docs/spec.md`; a transcrição da Entrega 06; as fotos de `docs/assets/estrutura/` | Gabriel |
| O portal do professor, a página de histórico e o dashboard, em [`portal-historico-e-dashboard.md`](portal-historico-e-dashboard.md) (entrega do dashboard, #52) | a Entrega 07 (`docs/entregas/entrega-07/`), atualizada para o estado final; R55 e R80 | Igor |
| Os testes e os resultados, em [`testes-e-resultados.md`](testes-e-resultados.md) (#69) | §8 da `docs/spec.md`; os comentários das rodadas na placa (#42, #44, #46, #48 e #50) | ninguém |
| As limitações e os caminhos descartados, em [`limitacoes-e-descartes.md`](limitacoes-e-descartes.md) (#70) | R3, R13, R14, R33, R50, R60, os limites declarados da R76, da R78 e da R79, o toque acima de 200 ms (R82) e os cortes do §1 da `docs/spec.md` | Cauã |
| Resumo, considerações finais, referências e quadro de quem fez o quê, sem arquivo (consolidação, #71) | as seções acima; o Quadro 7 da Entrega 06 | ninguém |

O esqueleto (#65) ficou com o Cauã, e a montagem do PDF (#72), com o Gabriel. Para pegar uma seção, atribua a issue a você, comente "peguei" e troque "ninguém" pelo seu nome nesta tabela.

## Regras para quem escreve

- Escreva só no arquivo da sua seção. O bloco de citação do topo de cada arquivo diz o que entra, a fonte e o que fica de fora; ele sai na consolidação.
- Siga o `docs/padrao-editorial.md`: primeira pessoa do plural ("decidimos"), sem travessão, e issue com o nome por extenso ("a busca da turma (#43)", e não "a #43" sozinha). O código de decisão da spec (R74, por exemplo) não aparece no texto, que diz a decisão por extenso.
- Fato que não está na `docs/spec.md` nem veio do Caio não entra: pergunte na issue.
- Medição e resultado de teste moram só na seção dos testes (#69), para o mesmo número não aparecer em duas versões. As outras seções descrevem o que o aparelho foi feito para fazer.
- Limitação se analisa só na seção das limitações (#70). As outras seções podem citar um limite em uma frase, ao lado da decisão que o gerou, como faz a do firmware.
