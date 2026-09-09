# Como o professor volta ao painel depois da troca de rádio

Type: prototype
Status: ready-for-human
Parent: map.md
Blocked by: —
Prazo real: 21/09 (item "código inicial preparado" do checklist do professor)
Discussão do grupo: https://github.com/caioplaninschek/presente/issues/3
Versão em linguagem simples: docs/pauta-do-grupo.md — Parte 3, ponto 8.

## Question

Levantada pelo João em 08/09. O R14 manda sincronizar durante o login: o aparelho valida a senha, responde "carregando turma…", derruba o AP, vai a STA buscar o roster e volta. A spec (§6, passos 5 e 6) afirma que o painel abre com a lista em cache, mas **não diz por qual mecanismo** o professor volta ao painel em vez de cair de novo na tela de login.

A resposta é conhecida e não precisa de decisão: a sessão vive no cookie que o navegador do professor guarda, não na conexão. O que **precisa** ser decidido e verificado é o que cerca isso:

1. **Ordem de operação.** O `Set-Cookie` da resposta do login tem que chegar inteiro ao celular **antes** de `softAPdisconnect`. Hoje o §5 (`net.cpp`) fixa a sequência da troca de rádio, mas não amarra essa sequência à conclusão da resposta HTTP. Vira requisito explícito, ou fica como cuidado de implementação?

2. **Onde o cookie mora.** Se o professor logar dentro do Captive Network Assistant (a janelinha do iOS/Android), o cookie fica no armazenamento daquele mini-navegador, que o sistema fecha por conta própria. Reabrir em Safari/Chrome pode significar login de novo. Opções: (a) manter tudo no CNA; (b) usar o CNA só para trazer o professor e seguir em `192.168.4.1` no navegador normal; (c) abandonar o cookie e carregar o token no próprio endereço da página, imune a qualquer janela ser fechada.

3. **Manter o celular preso ao AP.** Na reassociação, o sistema refaz o teste de conectividade (`captive.apple.com`, `generate_204`). Se o DNS continuar sequestrando tudo, o celular marca "rede sem internet" e pode migrar para o 4G sozinho — levando o painel junto, no meio da aula. Responder o probe de modo que o sistema considere a rede "assinada" resolve, mas muda o comportamento do portal cativo e precisa ser decidido junto com o item 2.

4. **Como a página volta.** A página que mostra "carregando turma…" precisa sondar o aparelho até ele responder. Intervalo, tempo limite, e o que exibir se estourar.

## Notas

`prototype` porque nada disso se decide em conversa: o comportamento muda entre iPhone e Android e entre versões de sistema. O artefato é um firmware mínimo — SoftAP + portal + login + troca de rádio + volta — sem RC522, sem LED e sem Supabase real, testado em pelo menos um iPhone e um Android.

**Este é o candidato natural a primeira fatia vertical do projeto.** Ele ataca o R10 (principal desafio técnico) isoladamente, só precisa do ESP32 que já está em mãos, e é exatamente o "código inicial preparado" que o professor cobra em 21/09. Se funcionar, o resto do firmware é montagem sobre um esqueleto que já se provou.

Resolvido quando existir: a sequência de rede amarrada à resposta do login, a decisão dos itens 2 e 3 verificada nos dois sistemas, e o resultado escrito no §5 e no §6 da spec.
