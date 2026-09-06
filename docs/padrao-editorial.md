# Padrão editorial

Vale para todo texto que sai deste projeto: PDFs de entrega, relatório técnico final, `DIARIO.md`, roteiro do vídeo. Não vale para código nem para comentário de código.

O objetivo é simples de enunciar e difícil de cumprir: **o texto tem que parecer escrito por um aluno que entende do assunto**, não montado por uma máquina que sabe a forma de um documento acadêmico sem ter o que dizer.

## Fluxo de escrita

**1. Fixe o contrato do artefato antes de redigir.** Finalidade, público, formato, limite de páginas, registro, fatos obrigatórios, sentido que não pode se perder, tokens literais (comandos, pinos, nomes de arquivo), e o que está incerto. Falta de instrução se resolve de forma conservadora: **não invente fato, convenção ou número**. Se um dado não existe na spec nem foi dado pelo Caio, pergunte — não preencha.

**2. Extraia as proposições, depois escreva.** Liste o que o texto precisa afirmar antes de montar frase. Redija a partir dessa lista, não parafraseando a spec. Quando faltar espaço, **corte proposições inteiras de propósito**; não amasse predicados distintos numa coordenação que troca o papel de cada um.

**3. Deixe o primeiro rascunho de lado e revise como se fosse de outra pessoa.** Confira o contrato primeiro; depois desça à sintaxe e ao vocabulário, frase a frase.

**4. Reescreva a partir das proposições, não por troca de sinônimo.** Se o esqueleto da frase está defeituoso, jogue o esqueleto fora em vez de remendá-lo. Redistribua as proposições entre orações quando o rascunho empilhar rótulos ou cair num contraste de fábrica.

**5. Revise de novo.** Está pronto quando a prosa soa idiomática **e** o artefato ainda cumpre a função pedida — tom, formalidade, extensão e estrutura.

**6. Entregue o texto, não comentário sobre o texto.** Sem "aqui está", sem justificativa das escolhas, a menos que peçam.

## Preservar sentido e responsabilidade

- Preserve fato, tese, intenção, certeza, **incerteza**, emoção, atribuição, compromisso e escopo.
- **Preserve a modalidade exatamente.** O que o aparelho *pode* fazer não vira o que ele *faz* nem o que ele *deve* fazer. Comportamento observado não vira recomendação. Este é o erro mais caro num documento técnico: a spec diz "o professor pode lançar presença manual" e o PDF sai afirmando que "o sistema lança presenças manualmente".
- Expresse a modalidade com a construção mais simples que funcione. Não fuja para nominalização ("a realização do lançamento") só para não escrever "lançar".
- **Preserve o predicado que dá papel a cada fato.** Não faça um verbo só governar coisas diferentes para encurtar a frase.
- Não intensifique, não suavize e não resolva ambiguidade que pertence à fonte. Se a spec diz "provavelmente", o PDF não diz "certamente".
- **Tokens literais ficam intactos:** comandos, caminhos, nomes de arquivo, pinos (GPIO 34), identificadores, mensagens de erro. Não "melhore" nenhum deles.

## Termos técnicos

- **Mantenha o termo estrangeiro** quando ele é o termo consagrado do domínio ou quando traduzir perderia precisão: *firmware*, *heap*, *handshake*, *debounce*, *hotspot*.
- **Prefira o termo em português** quando ele for igualmente preciso e natural: *chamada* e não *roll call*, *crachá* e não *badge*.
- **Integre o termo mantido à sintaxe portuguesa.** Artigo, gênero, número e preposição saem do conceito português que o termo designa, não do inglês. Uma interface é *o CLI*; uma habilidade é *uma skill*.
- **Não reproduza empilhamento de substantivo do inglês.** O nome próprio e o substantivo genérico formam sintagma português, com a preposição explícita:
  - ✅ a API do Supabase · ❌ a API Supabase · ❌ o Supabase API
  - ✅ o núcleo do Arduino · ❌ o Arduino Core (salvo quando for nome exato de produto)
  - ✅ o módulo RC522 · ✅ a placa ESP32
- **Explique um termo mantido só quando o leitor precisar**, na primeira ocorrência. O professor da disciplina não precisa que "SPI" seja explicado; "roster" precisa, porque é nome interno do projeto.

## Vícios que denunciam texto gerado

Nenhum destes aparece no texto final:

- **Muleta de abertura:** "Vale ressaltar que", "É importante destacar que", "Neste contexto", "Nesse sentido", "Cabe salientar".
- **Frase que anuncia a seção em vez de dizer algo:** "Nesta seção serão apresentados os componentes". O título já disse. Comece pelo conteúdo.
- **Tricolon automático:** "robusto, escalável e eficiente". Se os três adjetivos não carregam informação distinta, sobra um. Ou os três.
- **Contraste de fábrica:** "não apenas X, mas também Y"; "mais do que Z, trata-se de W".
- **Superlativo vazio:** "solução inovadora", "de forma eficiente", "tecnologia de ponta", "significativamente". Substitua por número ou corte.
- **Parágrafo de conclusão que repete o parágrafo anterior** com outras palavras.
- **Travessão em excesso.** Um por parágrafo já é muito.
- **Voz passiva por reflexo:** "foi realizada a implementação" em vez de "implementamos".
- **Gerundismo:** "estaremos utilizando" em vez de "usaremos".

## Regra de aferição

Antes de entregar, leia em voz alta um parágrafo ao acaso. Se ele puder ser colado em qualquer outro trabalho de qualquer outra disciplina sem alteração, ele não diz nada — reescreva a partir das proposições.
