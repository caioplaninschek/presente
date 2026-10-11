# senha-professor.ps1 - cria ou troca a conta de um professor no data/professores.json,
# o arquivo com que o aparelho confere a senha do login (docs/spec.md, secao 6.1 e R47; #41).
#
# Uso, na pasta do repo:   powershell -ExecutionPolicy Bypass -File ferramentas\senha-professor.ps1
# Roda no Windows PowerShell 5.1 e no PowerShell 7. Pede id, usuario, nome e senha (a senha nao aparece).
# Depois, grave o portal na placa com o uploadfs (ambiente presente). O professores.json nao vai
# para o git (.gitignore), porque leva o salt e o hash de cada professor.
#
# O id precisa existir na tabela professores do banco (supabase/002-dados-de-teste.sql):
# senao, o Enviar do pacote 4 e recusado.
#
# O salt sao 16 bytes sorteados, em hexadecimal. O hash e o SHA-256 do texto do salt seguido da
# senha, em UTF-8, em hexadecimal minusculo: o mesmo calculo do src/presente/portal.cpp.
#
# Exemplo versionado: data/professores.example.json, conta prof-teste-1, senha senha-exemplo-prof.
# -Senha e -Salt existem so para a verificacao (mesma entrada, mesmo arquivo); no uso normal, deixe sem.

param(
  [string]$Id,
  [string]$Usuario,
  [string]$Nome,
  [string]$Senha,
  [string]$Salt,
  [string]$Arquivo = (Join-Path $PSScriptRoot '..\data\professores.json')
)

$ErrorActionPreference = 'Stop'

function Hex([byte[]]$bytes) {
  return -join ($bytes | ForEach-Object { $_.ToString('x2') })
}

function Sha256Hex([string]$texto) {
  $sha = [Security.Cryptography.SHA256]::Create()
  return Hex $sha.ComputeHash([Text.Encoding]::UTF8.GetBytes($texto))
}

function Json([string]$texto) {
  return '"' + $texto.Replace('\', '\\').Replace('"', '\"') + '"'
}

if ((Sha256Hex 'abc') -ne 'ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad') {
  Write-Host 'ERRO: o SHA-256 deste computador nao bateu com o vetor conhecido. Nada foi gravado.'
  exit 1
}

if (-not $Id) { $Id = Read-Host 'id do professor (o mesmo da tabela professores do banco)' }
if (-not $Usuario) { $Usuario = Read-Host 'usuario do login' }
if (-not $Nome) { $Nome = Read-Host 'nome, como aparece no painel' }
if (-not $Senha) {
  $Senha = (New-Object Net.NetworkCredential('', (Read-Host 'senha' -AsSecureString))).Password
  $repetida = (New-Object Net.NetworkCredential('', (Read-Host 'repita a senha' -AsSecureString))).Password
  if ($Senha -cne $repetida) {
    Write-Host 'ERRO: as duas senhas sao diferentes. Nada foi gravado.'
    exit 1
  }
}

# Os limites cabem nos documentos de tamanho fixo do firmware: o corpo do login tem 256 bytes,
# o /api/estado tem 512 e as dez contas cabem em 4096 (src/presente/). Contam bytes em UTF-8,
# como o firmware guarda: letra acentuada vale 2.
if (-not $Id -or -not $Usuario -or -not $Nome -or -not $Senha) {
  Write-Host 'ERRO: id, usuario, nome e senha sao obrigatorios. Nada foi gravado.'
  exit 1
}
$bytesUtf8 = [Text.Encoding]::UTF8
if ($bytesUtf8.GetByteCount($Id) -gt 32 -or $bytesUtf8.GetByteCount($Usuario) -gt 32 -or
    $bytesUtf8.GetByteCount($Nome) -gt 60 -or $bytesUtf8.GetByteCount($Senha) -gt 64) {
  Write-Host 'ERRO: id e usuario vao ate 32 caracteres, nome ate 60 e senha ate 64 (letra acentuada conta 2). Nada foi gravado.'
  exit 1
}
# Tab ou quebra de linha sairiam crus no JSON montado a mao, e o aparelho nao leria o arquivo.
if (($Id + $Usuario + $Nome) -match '[\x00-\x1f\x7f]') {
  Write-Host 'ERRO: id, usuario e nome nao podem ter tab nem quebra de linha. Nada foi gravado.'
  exit 1
}
if ($Salt) {
  if ($Salt -cnotmatch '^[0-9a-f]{32}$') {
    Write-Host 'ERRO: -Salt tem de ser 32 caracteres hexadecimais minusculos. Nada foi gravado.'
    exit 1
  }
} else {
  $bytes = New-Object byte[] 16
  [Security.Cryptography.RandomNumberGenerator]::Create().GetBytes($bytes)
  $Salt = Hex $bytes
}
$hash = Sha256Hex ($Salt + $Senha)

# Caminho relativo vale a partir da pasta do PowerShell, e nao da pasta do processo.
$Arquivo = $ExecutionContext.SessionState.Path.GetUnresolvedProviderPathFromPSPath($Arquivo)
$utf8 = New-Object Text.UTF8Encoding $false

$linhas = @()
if (Test-Path $Arquivo) {
  $texto = [IO.File]::ReadAllText($Arquivo, $utf8)
  if ($texto.Trim()) {
    try {
      $contas = $texto | ConvertFrom-Json
    } catch {
      Write-Host "ERRO: $Arquivo nao e JSON valido. Conserte ou apague o arquivo; nada foi gravado."
      exit 1
    }
    foreach ($c in $contas) {
      if ($c.id -ceq $Id) { continue }
      if ($c.usuario -ceq $Usuario) {
        Write-Host "ERRO: o usuario $Usuario ja e da conta $($c.id). Nada foi gravado."
        exit 1
      }
      $linhas += '{"id":' + (Json $c.id) + ',"usuario":' + (Json $c.usuario) + ',"nome":' + (Json $c.nome) +
        ',"salt":' + (Json $c.salt) + ',"hash":' + (Json $c.hash) + '}'
    }
  }
}
$linhas += '{"id":' + (Json $Id) + ',"usuario":' + (Json $Usuario) + ',"nome":' + (Json $Nome) +
  ',"salt":' + (Json $Salt) + ',"hash":' + (Json $hash) + '}'
if ($linhas.Count -gt 10) {
  Write-Host 'ERRO: o aparelho guarda ate 10 contas de professor. Nada foi gravado.'
  exit 1
}

[IO.File]::WriteAllText($Arquivo, "[`n  " + ($linhas -join ",`n  ") + "`n]`n", $utf8)
Write-Host "Conta $Id gravada em $Arquivo ($($linhas.Count) conta(s) no arquivo)."
Write-Host 'Lembrete: o id precisa existir na tabela professores do banco. Agora grave com o uploadfs.'
