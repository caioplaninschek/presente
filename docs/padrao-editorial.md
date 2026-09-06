# Padrão editorial

Vale para todo texto que sai deste projeto: PDFs de entrega, relatório técnico final, `DIARIO.md`, roteiro do vídeo. Não vale para código nem para comentário de código.

Este arquivo é a **camada operativa**: o que se aplica a este projeto, e a ordem em que se aplica. A norma completa está em `docs/referencia/`, e é ela que manda em caso de conflito:

| Referência | Autoridade sobre |
|---|---|
| `referencia/padrao-editorial-completo.md` | fluxo de composição, desconstrução do rascunho, construções artificiais, restrições por tipo de artefato |
| `referencia/gramatica-almeida.md` | referência, voz, gerúndio, particípio, escopo de modificador, repetição, período |
| `referencia/pontuacao.md` | **admissibilidade gramatical** da pontuação (Almeida §§947-975) |

Regra de arbitragem: a revisão editorial pode apontar excesso ou padrão artificial, mas **não pode chamar de errado o que a referência de pontuação admite**. Primeiro determine a função da vírgula, do travessão ou dos dois-pontos na frase; só depois julgue se a construção merece reescrita por razão editorial. Nada de meta de frequência.

## Contexto deste projeto

**Público:** o professor da disciplina, que avalia. Não é desenvolvedor buscando referência de API nem banca de mestrado.
**Registro:** acadêmico, direto, sem solenidade postiça. Primeira pessoa do plural quando a equipe é o agente ("implementamos", "decidimos").
**Tipos de artefato em jogo:** documentação (entregas técnicas, relatório) e resumo editado (`DIARIO.md`). Ver as seções correspondentes na referência completa.
**Fonte dos fatos:** `docs/spec.md`. Fato que não está lá nem foi dado pelo Caio **não existe** — pergunte, não preencha.

## Fluxo

1. **Fixe o contrato do artefato antes de redigir.** Finalidade, público, formato, limite de páginas, registro, fatos obrigatórios, sentido protegido, tokens literais e o que está incerto. Falta de instrução se resolve de forma conservadora.
2. **Extraia as proposições e sua hierarquia; depois componha.** Escreva a partir dessa lista, não parafraseando a spec frase a frase. Faltando espaço, corte proposições subordinadas de propósito; não amasse predicados distintos numa coordenação que troca o papel de cada um.
3. **Deixe o rascunho de lado e revise como se fosse de outro autor.** Contrato primeiro; depois a desconstrução silenciosa, frase a frase — esqueleto de orações, referente e gênero, escopo de modificador, classe lexical, colocação e hierarquia das coordenações. Os sete passos estão na referência completa.
4. **Reescreva a partir das proposições, não por troca de sinônimo.** Esqueleto defeituoso se joga fora; não se remenda.
5. **Revise outra vez.** Pronto é quando a prosa soa idiomática **e** o artefato ainda cumpre a função pedida — tom, formalidade, extensão, estrutura.
6. **Entregue o texto, não comentário sobre o texto.**

## Sentido e responsabilidade

- Preserve fato, tese, intenção, certeza, **incerteza**, atribuição, compromisso e escopo.
- **Preserve a modalidade exatamente.** O que o aparelho *pode* fazer não vira o que ele *faz* nem o que ele *deve* fazer; comportamento observado não vira recomendação. É o erro mais caro aqui: a spec diz "o professor pode lançar presença manual" e o PDF sai afirmando que "o sistema lança presenças manualmente".
- Expresse a modalidade com a construção mais simples que funcione. Não fuja para nominalização ("a realização do lançamento") só para não escrever "lançar".
- Preserve o predicado que dá papel a cada fato. Não faça um verbo só governar coisas diferentes para encurtar a frase.
- Não intensifique, não suavize e não resolva ambiguidade que pertence à fonte.
- **Tokens literais ficam intactos**, caractere por caractere: comandos, caminhos, nomes de arquivo, pinos (`GPIO 34`), identificadores, mensagens de erro.

## Termos técnicos

- **Mantenha o estrangeiro** quando for o termo consagrado do domínio ou quando traduzir perder precisão: *firmware*, *heap*, *handshake*, *debounce*, *hotspot*.
- **Prefira o português** quando for igualmente preciso e natural: *chamada*, *crachá*, *lista da turma*.
- **Integre à sintaxe portuguesa.** Artigo, gênero, número e preposição saem do conceito português que o termo designa. Uma interface é *o CLI*; uma habilidade é *uma skill*.
- **Gênero de nome próprio vem do uso estabelecido, não do predicado da frase.** *O Google é uma empresa* não autoriza *a Google*. Sem uso verificável, explicite o núcleo comum (*a plataforma Supabase*) ou reescreva sem artigo. Não chute.
- **Não empilhe substantivo como em inglês.** Nome próprio e substantivo genérico formam sintagma português, com a preposição explícita:
  - ✅ a API do Supabase · ❌ a API Supabase · ❌ o Supabase API
  - ✅ o núcleo do Arduino · ✅ o módulo RC522 · ✅ a placa ESP32
- **Particípio descreve o que o substantivo sofreu.** Se só o mecanismo é automático, escreva *relatórios gerados e enviados automaticamente*, não *relatórios automatizados* (Almeida §938).
- **Explique um termo mantido só na primeira ocorrência e só se o leitor precisar.** O professor não precisa que "SPI" seja explicado; "roster" precisa, porque é nome interno do projeto — aliás, prefira *lista da turma*.

## Construções artificiais a remover

A lista completa está na referência. As que mais aparecem em texto acadêmico gerado:

- **Metadiscurso genérico:** "vale ressaltar que", "é importante destacar que", "neste contexto", "nesse sentido", e fórmulas de fechamento automáticas.
- **Frase que anuncia a seção em vez de dizer algo:** "Nesta seção serão apresentados os componentes". O título já disse.
- **Antítese de fábrica:** *não apenas X, mas também Y*; *não é X, mas Y*. Não introduza em rascunho novo. Contraste deliberado e exato de um autor competente se preserva; só o uso mecânico se reescreve — e a correção é reconstruir a relação entre as proposições, não trocar por *além de*.
- **Parataxe rítmica:** sequências de orações curtas, sobretudo em grupos de três, que soam polidas e deixam a relação entre as ideias por dizer. Vale igual para listas de rótulos paralelos.
- **Dois-pontos como golpe de efeito:** montagem que segura o ponto até um slogan depois dos dois-pontos. Enunciação e enumeração legítimas não são isso.
- **Antecipação fabricada:** "e é aqui que...", "agora vem a parte importante...".
- **Superlativo vazio:** "solução inovadora", "de forma eficiente", "significativamente". Troque por número ou corte.
- **Gerundismo** (*estaremos utilizando* por *usaremos*). Isso é uma perífrase de futuro calcada do inglês, não uma proibição do gerúndio: gerúndio com sujeito, tempo e função precisos está correto e não se mexe (Almeida §§942-944).
- **Nominalização burocrática:** "foi realizada a implementação do módulo" por "implementamos o módulo".
- **Argumento por formatação:** raciocínio virado em bullets só para ficar escaneável. Lista e tabela servem a material realmente enumerável ou comparável.
- **Parágrafo de conclusão que repete o anterior** com outras palavras.

## Aferição final

Leia em voz alta um parágrafo ao acaso. Se ele puder ser colado em qualquer outro trabalho de qualquer outra disciplina sem alteração, ele não diz nada — reescreva a partir das proposições. E se a ordem das palavras, as pausas ou a ênfase só soarem naturais quando reconstruídas mentalmente em inglês, o período está defeituoso.
