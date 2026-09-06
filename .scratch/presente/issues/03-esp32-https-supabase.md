# Caminho viável do ESP32 até o Supabase

Type: research
Status: claimed
Parent: map.md
Blocked by: —

## Question

A arquitetura inteira (R2, R3) depende de o ESP32 conseguir, em dois momentos por aula, conectar a um hotspot de celular e falar com a API REST do Supabase. Nada disso foi validado — é a maior aposta não testada da spec, e descobrir em outubro que não funciona não deixa tempo de reagir.

O que precisa ser respondido antes de qualquer código de rede:

1. **TLS.** O Supabase só aceita HTTPS. Quais as opções reais no Arduino Core (`setInsecure()`, `setCACert()` com raiz embutida, bundle do ESP-IDF), qual a prática recomendada e qual a realmente usada em protótipos?
2. **Memória.** Quanto de heap o handshake TLS consome, e isso convive com WebServer + DNSServer + LittleFS + ArduinoJson no mesmo binário? Há relatos de out-of-memory nesse combo?
3. **Bibliotecas.** `HTTPClient` + `WiFiClientSecure` é o caminho padrão, ou existe cliente Supabase para ESP32 que valha a pena?
4. **Hotspot de celular.** O ESP32 é só 2,4 GHz e hotspots modernos sobem em 5 GHz. Quais as pegadinhas em Android e iPhone, e como forçar 2,4 GHz?
5. **Alternância AP → STA → AP** (R4). Funciona de forma confiável? Quanto demora? Qual a sequência de chamadas correta e quais os bugs conhecidos?
6. **Plano B.** Se HTTPS direto se mostrar inviável, quais alternativas de custo zero existem (proxy HTTP, Cloudflare Worker, Apps Script)?

## Notas

`research` (AFK): a resposta está em documentação e em relato de terceiros, não no diretório de trabalho nem na cabeça do Caio. Subagente disparado em 06/09 00:07.

Desvio deliberado do padrão da skill: os achados vêm direto para este ticket, sem branch `research/<name>`. O repo tem 2 commits e nenhum remoto — uma branch descartável aqui é cerimônia sem ganho.

Este ticket bloqueia as decisões de esquema do Supabase e de escopo do dashboard em nuvem, porque as duas assumem que o caminho existe.
