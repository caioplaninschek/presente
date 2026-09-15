// secrets.example.h — modelo. Copie para secrets.h e preencha.
//
// secrets.h esta no .gitignore e NUNCA deve ser commitado.
// Este repositorio e publico: uma chave que subir aqui esta comprometida
// para sempre, mesmo que voce apague o commit depois.
//
//     cp include/secrets.example.h include/secrets.h

#pragma once

// --- Rede propria do aparelho (modo AP) -------------------------
// O professor conecta o celular nesta rede. A senha protege o ar da sala:
// sem ela, a senha individual do professor trafega legivel (decisao R6).
#define AP_SSID       "Presente-Sala01"
#define AP_PASSWORD   "troque-esta-senha"

// --- Hotspot de celular (modo STA) ------------------------------
// Usado nos dois momentos de internet: buscar a turma e enviar o relatorio.
// Lembre: o ESP32 e somente 2,4 GHz. Ver checklist R13d na spec.
#define STA_SSID      ""
#define STA_PASSWORD  ""

// SSID de reserva, gravado antes da apresentacao (risco residual 2 do
// ticket 03: hotspot subindo em 5 GHz no dia).
#define STA_SSID_FALLBACK      ""
#define STA_PASSWORD_FALLBACK  ""

// --- Supabase ---------------------------------------------------
// A chave publicavel (sb_publishable_...) fica dentro do binario de um
// aparelho fixo numa sala sem vigilancia. Por isso ela nao tem privilegio
// em tabela nenhuma: so executa as duas funcoes do banco (R33 e R35 na spec).
// Vai no cabecalho "apikey", nunca em "Authorization: Bearer".
#define SUPABASE_URL              "https://SEU-PROJETO.supabase.co"
#define SUPABASE_PUBLISHABLE_KEY  ""
