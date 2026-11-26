# APÊNDICE A – TRECHOS COMENTADOS DO CÓDIGO

Os três trechos abaixo vêm do repositório, e só mudamos os cortes, marcados com reticências depois de `//` ou de `--`, e a quebra de algumas linhas longas. Os comentários são os do próprio código, escritos sem acento.

No `setup()`, o aparelho liga o Wi-Fi no modo `WIFI_AP_STA`, em que a rede própria do aparelho e a conexão ao hotspot convivem no mesmo rádio, e põe a rede própria no ar com `WiFi.softAP()` (Código 1). O servidor de DNS responde a qualquer endereço com o IP do próprio aparelho, e é isso que faz o celular do professor abrir sozinho a página de login, como num portal cativo. O `loop()` só passa a vez a cada módulo: o DNS, a rede, o portal e o LED com o buzzer. Nenhum deles espera parado, porque cada um mede o tempo decorrido com `millis()`, e por isso o aparelho continua atendendo o portal enquanto o LED pisca.

**Código 1: O rádio no setup() e o laço principal, em src/presente/main.cpp**

```cpp
void setup() {
  // ... o serial, o LED, o leitor, o LittleFS e as contas
  WiFi.persistent(false);
  // o STA so entra no hotspot durante a busca da turma (net.cpp)
  WiFi.mode(WIFI_AP_STA);
  if (WiFi.softAP(AP_SSID, AP_PASSWORD)) {
    // ... a rede do aparelho no ar, com o endereco no serial
  }
  dns.start(PORTA_DNS, "*", WiFi.softAPIP());
  portal::iniciar(professores, montado);
  // ... a retomada da chamada depois de uma queda de energia
}

void loop() {
  dns.processNextRequest();
  // antes do portal, que ve o fim da busca na mesma volta
  net::atender();
  portal::atender();
  feedback::atualizar();
}
```

Fonte: elaborado pelos autores (2026).

O Código 2 é a decisão sobre cada crachá encostado, no módulo da chamada. Com a lista da turma, o UID lido se procura na lista, e o crachá que não está nela recebe o vermelho, sem nada gravado; sem a lista, no modo offline, o crachá se reconhece pelo próprio UID. O aluno que já estava presente recebe `JA_REGISTRADO`. Nos outros casos, o evento vai para o arquivo `/eventos.json` antes de o aluno entrar na memória de presentes e antes do sinal: se a gravação falha, ninguém é registrado, e o toque recebe o vermelho. Essa ordem garante que o relatório, montado a partir do arquivo, nunca perca uma presença que o aluno viu confirmada, e é também a causa suspeita, ainda não confirmada, dos toques que passaram dos 200 ms (Quadro 9).

**Código 2: A decisão sobre cada toque, em src/presente/chamada.cpp**

```cpp
Veredito decidir() {
  const char* chave = toque.uid;
  const char* matricula = nullptr;
  if (comLista) {
    const JsonObjectConst aluno = alunoPor("uid", toque.uid);
    matricula = aluno["matricula"];
    if (matricula == nullptr) {
      return Veredito::NAO_RECONHECIDO;  // fora da lista: nada gravado
    }
    toque.nome = aluno["nome"];
    toque.matricula = matricula;
    chave = matricula;
  }

  if (jaPresente(chave)) {
    return Veredito::JA_REGISTRADO;
  }
  // ... a memoria de presencas cheia tambem recusa o toque

  StaticJsonDocument<512> evento;
  evento["uid"] = static_cast<const char*>(toque.uid);
  // ... a matricula e o nome, com a lista, a hora e a origem "nfc"
  if (!gravar(evento, toque.uid)) {
    return Veredito::NAO_RECONHECIDO;
  }
  marcarPresente(chave);
  return Veredito::REGISTRADO;
}
```

Fonte: elaborado pelos autores (2026).

O Código 3 é o lado do banco. O aparelho não grava direto nas tabelas: no Enviar, ele chama a função `enviar_relatorio`, que roda no PostgreSQL do Supabase e grava a chamada e os eventos numa transação só. A função roda com os privilégios de quem a criou (`security definer`), e por isso a chave publicável do aparelho, que não tem privilégio em tabela nenhuma, grava só por ela. A chamada entra com `on conflict (id) do nothing`: se a mesma chamada já estava no banco, nada é gravado, e a função responde `ja_enviado`. É isso que deixa o professor repetir o Enviar, depois de uma falha de rede, sem duplicar o relatório. No modo offline, a mesma função completa a matrícula de cada evento a partir do UID do crachá.

**Código 3: O começo da função enviar_relatorio, em supabase/001-esquema.sql**

```sql
create function public.enviar_relatorio(relatorio jsonb)
returns jsonb
language plpgsql
security definer
set search_path = ''
as $$
declare
  -- ... o JSON da sessao, o sessaoId e os contadores
begin
  -- ... a recusa do relatorio sem sessaoId
  insert into public.sessoes (id, professor_id, turma, inicio, fim,
                               modo, relogio, estado)
  values (
    v_id,
    -- ... os outros campos da sessao, lidos do JSON do relatorio
  )
  on conflict (id) do nothing;

  if not found then
    return jsonb_build_object('resultado', 'ja_enviado');
  end if;
  -- ... os eventos e, no modo offline, a matricula completada pelo UID
```

Fonte: elaborado pelos autores (2026).
