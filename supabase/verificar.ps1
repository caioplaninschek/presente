#Requires -Version 7
# verificar.ps1 - confere o banco do Presente! do jeito que o aparelho o enxerga:
# com a chave publicavel e sem login (docs/spec.md, R33, R34, R35).
#
# Uso, de qualquer pasta:   pwsh -NoProfile -File <repo>/supabase/verificar.ps1
# Le SUPABASE_URL e SUPABASE_PUBLISHABLE_KEY de include/secrets.h e nunca imprime nenhum dos dois.
# Rode depois de aplicar qualquer arquivo de supabase/. Saida 0 = tudo PASS; 1 = alguma FAIL;
# 2 = falta configuracao; 3 = erro de rede.
#
# Cada rodada grava uma sessao "verificacao-TESTE01-..." na turma ficticia TESTE01.
# Ela sai junto com as aulas "teste-" na integracao (epic #9).

$ErrorActionPreference = 'Stop'

$secrets = Join-Path $PSScriptRoot '..' 'include' 'secrets.h'
if (-not (Test-Path $secrets)) {
  Write-Host 'FALTA include/secrets.h (copie de include/secrets.example.h e preencha)'
  exit 2
}
$texto = Get-Content -Raw -Encoding utf8 $secrets

function Ler-Define([string]$nome) {
  $m = [regex]::Match($texto, '#define\s+' + $nome + '\s+"([^"]*)"')
  if (-not $m.Success -or $m.Groups[1].Value -eq '' -or $m.Groups[1].Value -like '*SEU-PROJETO*') {
    Write-Host "FALTA $nome preenchido em include/secrets.h"
    exit 2
  }
  return $m.Groups[1].Value
}

$url = (Ler-Define 'SUPABASE_URL').TrimEnd('/')
$chave = Ler-Define 'SUPABASE_PUBLISHABLE_KEY'
$falhas = 0

function Chamar([string]$metodo, [string]$caminho, $corpo) {
  $params = @{
    Method             = $metodo
    Uri                = $url + $caminho
    Headers            = @{ apikey = $chave }
    SkipHttpErrorCheck = $true
    TimeoutSec         = 30
  }
  if ($null -ne $corpo) {
    $params.Body = ($corpo | ConvertTo-Json -Depth 10 -Compress)
    $params.ContentType = 'application/json; charset=utf-8'
  }
  try {
    $r = Invoke-WebRequest @params
  } catch {
    # A mensagem original traz o endereco do projeto: nao imprimir.
    Write-Host "ERRO de rede em $metodo $($caminho.Split('?')[0]) (detalhe omitido para nao expor o endereco)"
    exit 3
  }
  $conteudo = if ($null -eq $r.Content) { '' } elseif ($r.Content -is [byte[]]) { [Text.Encoding]::UTF8.GetString($r.Content) } else { [string]$r.Content }
  return [pscustomobject]@{ Status = [int]$r.StatusCode; Texto = $conteudo }
}

function Json($resposta) {
  try { return ($resposta.Texto | ConvertFrom-Json) } catch { return $null }
}

function Codigo($resposta) {
  $j = Json $resposta
  if ($null -eq $j) { return '-' }
  foreach ($campo in 'error_code', 'code') {
    if ($j.PSObject.Properties.Name -contains $campo -and $j.$campo) { return [string]$j.$campo }
  }
  return '-'
}

function Registrar([string]$id, [bool]$ok, [string]$detalhe) {
  if ($ok) {
    Write-Host "PASS $id - $detalhe"
  } else {
    Write-Host "FAIL $id - $detalhe"
    $script:falhas++
  }
}

function Agora([int]$minutos) {
  $t = [DateTimeOffset]::UtcNow.ToOffset([TimeSpan]::FromHours(-3)).AddMinutes($minutos)
  return $t.ToString("yyyy-MM-dd'T'HH:mm:sszzz", [Globalization.CultureInfo]::InvariantCulture)
}

# V1 - a lista da turma real: 40 ativos, ninguem so da TESTE01, nenhum inativo
$r = Chamar 'POST' '/rest/v1/rpc/roster' @{ turma = '4172CMPN6A_P1' }
$lista = @()
if ($r.Status -eq 200) { $lista = @(Json $r) }
$bytes = [Text.Encoding]::UTF8.GetByteCount($r.Texto)
$intrusos = @($lista | Where-Object { $_.matricula -like '12409901*' -or $_.matricula -eq '1240990036' }).Count
Registrar 'V1' ($r.Status -eq 200 -and $lista.Count -eq 40 -and $intrusos -eq 0) "roster da turma real: status $($r.Status), $($lista.Count) alunos, $intrusos de fora, $bytes bytes"

# V2 - turma que nao existe: lista vazia
$r = Chamar 'POST' '/rest/v1/rpc/roster' @{ turma = 'NAO-EXISTE' }
$vazia = @()
if ($r.Status -eq 200) { $vazia = @(Json $r) }
Registrar 'V2' ($r.Status -eq 200 -and $vazia.Count -eq 0) "roster de turma inexistente: status $($r.Status), $($vazia.Count) alunos"

# V3 - ler tabela direto: recusado (nem turma nenhuma, nem historico)
$ra = Chamar 'GET' '/rest/v1/alunos?select=matricula&limit=1' $null
$rs = Chamar 'GET' '/rest/v1/sessoes?select=id&limit=1' $null
Registrar 'V3' ($ra.Status -ge 400 -and $rs.Status -ge 400) "ler alunos: status $($ra.Status) ($(Codigo $ra)); ler sessoes: status $($rs.Status) ($(Codigo $rs))"

# V4 - enviar um relatorio: grava 2 eventos e o banco preenche 1 matricula
$rt = Chamar 'POST' '/rest/v1/rpc/roster' @{ turma = 'TESTE01' }
$alunoT = $null
if ($rt.Status -eq 200) { $alunoT = @(Json $rt) | Where-Object { $_.matricula -eq '1240990101' } | Select-Object -First 1 }
$sessaoId = 'verificacao-TESTE01-' + [DateTimeOffset]::UtcNow.ToOffset([TimeSpan]::FromHours(-3)).ToString('yyyyMMdd-HHmmss', [Globalization.CultureInfo]::InvariantCulture)
$relatorio = @{
  sessao = @{
    sessaoId    = $sessaoId
    professorId = 'prof-teste-2'
    disciplina  = 'Disciplina de Teste'
    turma       = 'TESTE01'
    inicio      = (Agora -5)
    fim         = (Agora 0)
    modo        = 'offline'
    relogio     = 'portal'
  }
  tempos           = @('t1', 't2')
  roster_carregado = $false
  eventos          = @(
    @{ uid = $alunoT.uid; timestamp = (Agora -4); origem = 'nfc'; enriquecido = $false },
    @{ matricula = '1240990102'; timestamp = (Agora -3); origem = 'manual'; lancado_por = 'prof-teste-2'; motivo = 'verificacao automatica' }
  )
}
if ($null -eq $alunoT -or -not $alunoT.uid) {
  Registrar 'V4' $false "nao achei o uid do aluno 1240990101 no roster da TESTE01 (status $($rt.Status))"
} else {
  $r = Chamar 'POST' '/rest/v1/rpc/enviar_relatorio' @{ relatorio = $relatorio }
  $j = Json $r
  $ok = $r.Status -eq 200 -and $j.resultado -eq 'gravado' -and $j.eventos -eq 2 -and $j.enriquecidos_no_banco -eq 1
  Registrar 'V4' $ok "enviar_relatorio: status $($r.Status), resposta $($r.Texto)"
}

# V5 - o mesmo relatorio de novo: ja_enviado, nada gravado (HU-39)
$r = Chamar 'POST' '/rest/v1/rpc/enviar_relatorio' @{ relatorio = $relatorio }
$j = Json $r
Registrar 'V5' ($r.Status -eq 200 -and $j.resultado -eq 'ja_enviado') "segundo envio: status $($r.Status), resposta $($r.Texto)"

# V6 - apagar evento: recusado
$r = Chamar 'DELETE' "/rest/v1/eventos?sessao_id=eq.$sessaoId" $null
Registrar 'V6' ($r.Status -ge 400) "apagar eventos: status $($r.Status) ($(Codigo $r))"

# V7 - alterar sessao: recusado
$r = Chamar 'PATCH' "/rest/v1/sessoes?id=eq.$sessaoId" @{ estado = 'cancelada' }
Registrar 'V7' ($r.Status -ge 400) "alterar sessao: status $($r.Status) ($(Codigo $r))"

# V8 - criar conta: recusado (cadastro publico desligado)
$email = 'verificacao-' + [guid]::NewGuid().ToString('N').Substring(0, 12) + '@example.com'
$senha = [guid]::NewGuid().ToString('N')
$r = Chamar 'POST' '/auth/v1/signup' @{ email = $email; password = $senha }
Registrar 'V8' ($r.Status -ge 400) "criar conta: status $($r.Status) ($(Codigo $r))"

Write-Host ''
Write-Host "sessao de verificacao: $sessaoId"
if ($falhas -eq 0) {
  Write-Host 'RESULTADO: 8 PASS'
  exit 0
}
Write-Host "RESULTADO: $falhas FAIL"
exit 1
