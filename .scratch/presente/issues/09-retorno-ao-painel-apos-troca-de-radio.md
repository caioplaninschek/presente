# Como o professor volta ao painel depois da troca de rádio

Type: prototype
Status: resolved
Resolvido em: 23/09/2026, pela R49 (opção (a), com a retomada pelo login); a pergunta 3 se confere na placa nas tarefas #42 e #50 do GitHub
Dono: Caio (frente "Rede" do §9 da spec; assumido na issue #3 em 10/09/2026)
Parent: map.md
Blocked by: ~~placa física — o ESP32 está com o João; combinado de passar na segunda, 15/09~~ — a placa não circula (R23, 11/09). ~~Hoje: os testes 7d (#17) e 7c (#18), que o João roda, e o veredito (#19)~~ — os três fecharam em 22/09 (R45). Hoje: o portal do professor rodando dentro do aparelho, na integração (epic #9)
Prazo real: ~~21/09 (item "código inicial preparado" do checklist do professor)~~ o veredito saiu em 22/09; os itens 2 e 3 fecham na integração, com a data do dashboard (epic #9) ainda a confirmar
Discussão do grupo: https://github.com/caioplaninschek/presente/issues/3
Versão em linguagem simples: docs/pauta-do-grupo.md — Parte 3, ponto 8.

## Question

Levantada pelo João em 08/09. O R14 manda sincronizar durante o login: o aparelho valida a senha, responde "carregando turma…", derruba o AP, vai a STA buscar o roster e volta. A spec (§6, passos 5 e 6) afirma que o painel abre com a lista em cache, mas **não diz por qual mecanismo** o professor volta ao painel em vez de cair de novo na tela de login.

A resposta é conhecida e não precisa de decisão: a sessão vive no cookie que o navegador do professor guarda, não na conexão. O que **precisa** ser decidido e verificado é o que cerca isso.

⚠️ **Atualizado em 10/09 (R19): o escopo cresceu e a pergunta de fundo mudou.** O João apontou na issue #3 que `WIFI_AP_STA` é nativo do ESP32, e ele está certo — R4 partia de premissa errada. O protótipo deixa de validar um arranjo e passa a **comparar dois**:

- **Plano A — `WIFI_AP_STA`.** AP e STA no ar ao mesmo tempo, no mesmo canal (o do STA), com CSA anunciando a migração. O professor nunca perde a conexão, e os itens 1 e 4 abaixo deixam de existir: não há `softAPdisconnect` para ordenar, nem "carregando turma…" para sondar. Sobram os itens 2 e 3, que são do portal cativo e independem do arranjo de rádio.
- **Plano B — alternância `AP → STA → AP`.** O que o ticket descrevia até aqui, e o que a spec ainda especifica. Entra se o A reprovar.

**Os dois critérios que decidem (testes 7c e 7d do §8):**

1. O celular do professor sobrevive ao pulo de canal, **em iPhone e em Android**? Se cair, reconecta sozinho em menos de 5 s?
2. Sobram **≥40 KB de heap** no instante do handshake TLS, com AP + DNS + WebServer + roster carregados? Logar `ESP.getFreeHeap()` imediatamente antes do `connect()`.

Reprovou qualquer um → Plano B, e o ticket segue como estava escrito. Aprovou os dois → R4 é reescrito, e somem do projeto a transição de 5–15 s, o R15 (recuperação quando o AP não volta) e boa parte do risco do §10.

O que segue valendo nos dois planos:

1. **Ordem de operação.** O `Set-Cookie` da resposta do login tem que chegar inteiro ao celular **antes** de `softAPdisconnect`. Hoje o §5 (`net.cpp`) fixa a sequência da troca de rádio, mas não amarra essa sequência à conclusão da resposta HTTP. Vira requisito explícito, ou fica como cuidado de implementação?

2. **Onde o cookie mora.** Se o professor logar dentro do Captive Network Assistant (a janelinha do iOS/Android), o cookie fica no armazenamento daquele mini-navegador, que o sistema fecha por conta própria. Reabrir em Safari/Chrome pode significar login de novo. Opções vivas: (a) manter tudo no CNA; ~~(b) usar o CNA só para trazer o professor e seguir em `192.168.4.1` no navegador normal.~~ Derrubada em 23/09 pela R49: com a retomada pelo login, o cookie perdido custa só digitar a senha de novo. ~~(c) abandonar o cookie e carregar o token no próprio endereço da página~~ — ⛔ **derrubada em 10/09 na issue #3**, por objeção do Cauã aceita pelo Caio: credencial no endereço fica exposta em histórico de navegador, em captura de tela e em registro de acesso de servidor. Registrada na spec em 11/09 como **R25**; não voltar a listá-la como alternativa.

3. **Manter o celular preso ao AP.** Na reassociação, o sistema refaz o teste de conectividade (`captive.apple.com`, `generate_204`). Se o DNS continuar sequestrando tudo, o celular marca "rede sem internet" e pode migrar para o 4G sozinho — levando o painel junto, no meio da aula. Responder o probe de modo que o sistema considere a rede "assinada" resolve, mas muda o comportamento do portal cativo e precisa ser decidido junto com o item 2.

4. **Como a página volta.** A página que mostra "carregando turma…" precisa sondar o aparelho até ele responder. Intervalo, tempo limite, e o que exibir se estourar.

   ⚠️ **Atualizado em 15/09 (R40), com o protótipo da #20 na mão.** O Igor respondeu os três números como **valores de partida**: sonda de 1 em 1 s, desiste em 25 s e, ao desistir, oferece seguir sem a lista — que é o modo offline que o §5 já previa. A conferência achou o defeito que isso escondia: o botão *Seguir sem a lista* pedia `/api/seguir-offline` e só trocava de tela dentro do `.then()`, de modo que **com o aparelho mudo — exatamente o caso em que a tela existe — ele não fazia nada e não avisava nada**. O *Voltar ao login* tinha o mesmo desenho. A **R40** fechou a parte que é do portal: todo pedido ao aparelho tem tempo-limite, a falha aparece na tela, o botão volta a aceitar toque e nenhuma tela troca sem confirmação.

   **O que continua deste item:** os três números só se calibram na bancada, com o rádio de verdade saindo do ar; e sobra a pergunta de firmware que a R40 não responde — como o aparelho confirma "seguir sem a lista" se ele é justamente quem não está respondendo. Fecha com o portal rodando dentro do aparelho, depois da Entrega 05.

✅ **Veredito em 22/09 (R45, issue #19): vale o Plano A.** A memória passou em 21/09 (236,5 KB, e 229,6 KB com um celular conectado), e o painel passou em 22/09, na sala, com iPhone e Android, pelo relato do grupo e sem log. Com isso, o item 1 deixa de existir e o item 4 fica só com a calibração dos tempos. O ticket continua `in-progress` pelos itens 2 e 3, que fecham quando o portal rodar dentro do aparelho.

⚠️ **Atualizado em 13/09: a execução deste protótipo foi quebrada em cinco issues** — firmware (#15), roteiro (#16), teste 7d (#17), teste 7c (#18) e veredito (#19). A #19 resolve o veredito entre os planos, com três desfechos possíveis (Plano A, Plano A provisório pela R32 ou Plano B). Os itens 2 e 3 **não têm teste** antes da Entrega 05: a #18 só observa onde o login abriu e se o celular trocou sozinho para o 4G, e a escolha está declarada como lacuna no §8 da spec.

## Notas

`prototype` porque nada disso se decide em conversa: o comportamento muda entre iPhone e Android e entre versões de sistema. O artefato é um firmware mínimo — SoftAP + portal + login + rede + volta — sem RC522, sem LED e sem banco de verdade, testado em pelo menos um iPhone e um Android (ou com veredito parcial, R24 e R32). A conexão segura, porém, abre contra o host real do Supabase, para a medição de memória valer (aresta 07 → 05 do `.scratch/prototipo/graph.md`, 11/09). Ordem de execução: montar o **Plano A** primeiro (é o mais curto: sem sequência de desligamento, sem recuperação de AP) e só descer para o B se algum dos dois critérios reprovar.

**Este é o candidato natural a primeira fatia vertical do projeto.** Ele ataca o §10 (principal desafio técnico) isoladamente, só precisa do ESP32 que já está em mãos, e é exatamente o "código inicial preparado" que o professor cobra em 21/09. Se funcionar, o resto do firmware é montagem sobre um esqueleto que já se provou.

Resolvido quando existir: o veredito entre Plano A e Plano B com as duas medidas registradas (comportamento do celular no pulo de canal e heap livre no handshake), a decisão dos itens 2 e 3 verificada nos dois sistemas, e o resultado escrito no §5, no §6 e no R4 da spec. Desde 13/09 o fechamento é em duas partes: o veredito sai na #19, e os itens 2 e 3 fecham depois da Entrega 05, quando o portal rodar dentro do aparelho. Evidência gravada, conforme a política do §8 — log serial com o número do heap e vídeo curto do painel sobrevivendo à sincronização.

✅ **Resolvido em 23/09 (R49):** o professor fica na janela do portal cativo, e um novo login volta à chamada aberta, porque ela vive no aparelho. O item 4 segue na tarefa das mensagens entre o portal e o aparelho (#36), e o item 3 se observa na placa.
