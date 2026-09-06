# Caminho viável do ESP32 até o Supabase

Type: research
Status: resolved
Parent: map.md
Blocked by: —
Resolvido: 2026-09-06

## Question

A arquitetura inteira (R2, R3) depende de o ESP32 conseguir, em dois momentos por aula, conectar a um hotspot de celular e falar com a API REST do Supabase. Nada disso foi validado — é a maior aposta não testada da spec, e descobrir em outubro que não funciona não deixa tempo de reagir.

O que precisa ser respondido antes de qualquer código de rede:

1. **TLS.** O Supabase só aceita HTTPS. Quais as opções reais no Arduino Core (`setInsecure()`, `setCACert()` com raiz embutida, bundle do ESP-IDF), qual a prática recomendada e qual a realmente usada em protótipos?
2. **Memória.** Quanto de heap o handshake TLS consome, e isso convive com WebServer + DNSServer + LittleFS + ArduinoJson no mesmo binário? Há relatos de out-of-memory nesse combo?
3. **Bibliotecas.** `HTTPClient` + `WiFiClientSecure` é o caminho padrão, ou existe cliente Supabase para ESP32 que valha a pena?
4. **Hotspot de celular.** O ESP32 é só 2,4 GHz e hotspots modernos sobem em 5 GHz. Quais as pegadinhas em Android e iPhone, e como forçar 2,4 GHz?
5. **Alternância AP → STA → AP** (R4). Funciona de forma confiável? Quanto demora? Qual a sequência de chamadas correta e quais os bugs conhecidos?
6. **Plano B.** Se HTTPS direto se mostrar inviável, quais alternativas de custo zero existem (proxy HTTP, Cloudflare Worker, Apps Script)?

## Answer

**O caminho é viável. `HTTPClient` + `WiFiClientSecure` + `setInsecure()`, sem biblioteca de Supabase, sem NTP, sem certificado pinado.**

**1. TLS — `setInsecure()`, decidido.** Existem três opções (`setInsecure()`, `setCACert()`, `setCACertBundle()`). Pinar certificado está descartado por um motivo factual: a Supabase **não usa uma CA só** e troca de emissor sem aviso. Verificação do handshake em 06/09/2026 mostrou três cadeias diferentes convivendo — `supabase.co` por Let's Encrypt (ISRG Root YR), `realtime.supabase.co` pela Amazon (Amazon Root CA 1), `api.supabase.com` pelo Google (GTS Root R4). Somado a isso, a Let's Encrypt migrou de hierarquia em 2026. Certificado pinado é bomba-relógio: quebra sozinho num sábado de outubro e ninguém vai saber por quê. `setCACert()` ainda exigiria NTP com relógio correto, senão falha com `-0x2700`.

Custo assumido: `setInsecure()` **criptografa mas não autentica o servidor**. Um MITM no hotspot é teoricamente possível. Proporcional aqui — o hotspot é do próprio grupo, a chave é anon e descartável, e é protótipo acadêmico. Registrar essa limitação no relatório técnico é mais honesto e vale mais nota que fingir que não existe.

**2. Memória — viável, com condição.** O handshake pede 40–50 KB de heap livre; o mbedTLS aloca 16 KB de RX + 16 KB de TX por padrão. Conviver com WebServer + DNSServer + LittleFS + ArduinoJson é apertado mas funciona **desde que o AP seja fechado antes de abrir o TLS**. Isso converte R4 (AP e STA nunca simultâneos) de preferência em **requisito de memória**, não só de canal de rádio.

Armadilha achada: `WiFiClientSecure` vaza ~4 KB por `connect()` **falho** (arduino-esp32 #3808). Sem limite de tentativas, o retry come o heap até travar o aparelho. Constraint de firmware, não detalhe.

**3. Bibliotecas — HTTP cru.** A `jhagas/ESPSupabase` está parada desde 16/07/2025 (~14 meses), arrasta `WebSockets` como dependência morta para quem não usa Realtime, e internamente só faz `setInsecure()` + `HTTPClient` + `getString()`. Não entrega nada que 20 linhas próprias não entreguem, e acopla o projeto a código sem manutenção.

Detalhe de implementação: usar `getStream()` no GET do roster (~3 KB), não `getString()` — `getString()` em resposta grande fragmenta o heap.

**4. Hotspot — resolvido por checklist.** iPhone: ligar **"Maximizar Compatibilidade"** nas configurações do Ponto de Acesso (força 2,4 GHz) e manter a tela do hotspot aberta durante o pareamento, porque o iOS suspende o beacon. Evitar acento e caractere especial no nome do iPhone, que vira o SSID. Android: escolher a banda 2,4 GHz explicitamente; senha longa ou com caractere especial já causou falha relatada.

**5. Alternância AP→STA→AP — funciona, e é o ponto mais frágil do plano.** Bugs conhecidos: clientes que não reconectam ao softAP na segunda vez (#4294), AP que não volta depois de `softAPdisconnect()` (esp32.com t=37829), `esp_wifi_get_mode()` retornando `WIFI_MODE_NULL` após a troca (#2752).

Sequência que funciona:

```cpp
server.stop(); dns.stop();
WiFi.softAPdisconnect(true); WiFi.mode(WIFI_OFF); delay(500);
WiFi.mode(WIFI_STA); WiFi.begin(ssid, pass);
// ... trabalho ...
WiFi.disconnect(true); WiFi.mode(WIFI_OFF); delay(500);
WiFi.mode(WIFI_AP); WiFi.softAP(ssid, pass);
dns.start(...); server.begin();
```

⚠️ **Correção da spec:** a transição leva **5–15 s**, não os ~3 s que R4 estimava. `WiFi.begin()` sozinho já consome segundos.

Confirmado que `WIFI_AP_STA` (os dois juntos) evita a troca mas **é pior**: o softAP muda de canal para acompanhar o STA e derruba quem estiver conectado. R4 estava certa na direção.

**6. Plano B — não é necessário, e os candidatos são piores.** Cloudflare Worker **não remove o TLS** (workers.dev não serve HTTP puro). Apps Script é HTTPS-only e ainda exige `setFollowRedirects()` por causa do 302 — troca a dor do Supabase por outra. A única opção HTTP puro é um proxy rodando num notebook na sala, o que exige o notebook ligado durante a aula. Nenhuma compensa.

## Riscos residuais

1. **A alternância de rádio é o ponto de falha mais provável do projeto.** Mitigação sugerida pela pesquisa: se a volta ao AP falhar, `ESP.restart()` — em aula, reboot de 2 s é aceitável, desde que o estado esteja persistido em LittleFS antes da troca. Isso virou o ticket **Comportamento do rádio: quando sincronizar e o que acontece quando a troca falha**, porque envolve decisão de produto, não só de código.
2. **Hotspot subindo em 5 GHz no dia da apresentação.** Mitigação: checklist impresso e um SSID de fallback já gravado no aparelho.

## Notas

`research` (AFK), subagente disparado 06/09 00:07, concluído 00:12.

Desvio deliberado do padrão da skill: achados gravados direto no ticket, sem branch `research/<name>` — repo com 3 commits e sem remoto, branch descartável seria cerimônia sem ganho.
