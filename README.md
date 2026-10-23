# Presente!

Terminal embarcado que registra presença em sala de aula por aproximação de tag NFC.

Projeto integrador da disciplina **Sistemas Embarcados** — Universidade Veiga de Almeida, campus Barra, 2026.2.

## O problema

O professor gasta de 10 a 15 minutos por aula fazendo chamada manual no sistema da UVA: lê nome por nome, pergunta à turma, marca a caixa. É tempo de aula perdido, e um deslize de linha marca falta no aluno errado.

## A solução

Um aparelho fixo ao lado da porta. O professor conecta o celular na rede Wi-Fi do próprio aparelho, faz login, e abre a chamada. O aluno encosta o crachá, ouve o bip, e está presente. No fim, o relatório sobe para a nuvem — ou é exportado para o celular do professor, se a internet falhar.

## Como está montado

```
[Tag NFC] → [RC522] → [ESP32] → [LED RGB + Buzzer]
                          ↓
        modo AP: portal do professor (celular)
        modo STA: Supabase (roster + relatório)
        os dois no ar ao mesmo tempo, no mesmo canal (R45)
```

**Sensor:** RC522 (leitura de UID 13,56 MHz).
**Atuadores:** LED RGB e buzzer, acionado por um transistor TIP122 (spec, R44).
**Firmware:** C++ sobre Arduino Core, build com PlatformIO. Versões fixadas no `platformio.ini`, e cada programa de bancada com a sua pasta e o seu ambiente (spec, R28 e R29).

## Documentação

| Onde | O quê |
|---|---|
| [`docs/ambiente-de-trabalho.md`](docs/ambiente-de-trabalho.md) | Como preparar a máquina: clone, IA lendo o contexto do projeto, rotina de `git pull` |
| [`docs/spec.md`](docs/spec.md) | Especificação completa: decisões, arquitetura, contrato de dados, plano de testes |
| [`docs/padrao-editorial.md`](docs/padrao-editorial.md) | O padrão de escrita de todo texto que sai do projeto: entregas, relatório, diário, roteiro de vídeo |
| [`docs/pauta-do-grupo.md`](docs/pauta-do-grupo.md) | A pauta de conversa de 08 a 10/09, já encerrada; as datas do corpo são as da época |
| [`docs/entregas.md`](docs/entregas.md) | Os enunciados do professor e o que saiu em cada entrega |
| [`docs/entregas/`](docs/entregas/) | Uma pasta por entrega: o PDF entregue, a transcrição e o rascunho com o texto integral. A pasta `entrega-07` não foi entregue e é base da entrega final *(14/10, R72)* |
| [`docs/datasheets/`](docs/datasheets/) | Os datasheets do ESP32-WROOM-32 e do MFRC522, em PDF e transcritos, com as figuras |
| [`DIARIO.md`](DIARIO.md) | Diário de bordo semanal |
| [`.scratch/presente/map.md`](.scratch/presente/map.md) | Mapa das decisões de projeto, abertas e fechadas |
| [`.scratch/integracao/graph.md`](.scratch/integracao/graph.md) | ~~Registro de execução desde 23/09: as tarefas até a entrega final, o grafo de dependência e o de-para com as Issues~~ *(30/09)* O grafo de dependência das tarefas até a entrega final; cada tarefa vive na sua issue |
| [`.scratch/prototipo/graph.md`](.scratch/prototipo/graph.md) | Registro de execução até a demonstração de 22/09, já fechado |
| [Issues](https://github.com/caioplaninschek/presente/issues) | O fórum do grupo e o enunciado completo de cada tarefa — passo a passo, o que tem que aparecer, o que fazer se der errado |
| [`AGENTS.md`](AGENTS.md) | Convenções do repositório |

Onde mora o código:

| Onde | O quê |
|---|---|
| `src/<nome>/` | Um programa de bancada por pasta, com o ambiente de mesmo nome no `platformio.ini` (R28) |
| `src/presente/` | O firmware de verdade, ambiente `presente`: os pacotes 1 a 5 (o portal e o login, a lista da turma pela internet, a chamada, o relatório com o Enviar e o Encerrar, e a volta depois de uma queda de energia) |
| `lib/<módulo>/` | O que dois programas dividem, com o nome do módulo do §5 da spec: `rfid`, `feedback`, `storage` |
| `lib/config/` | Pinos e tempos do aparelho num lugar só, fora os três pinos do barramento do leitor, que o `SPI.begin()` já define (R66) |
| `data/` | O portal do professor (HTML, CSS e JS). É desta pasta que sai a imagem de filesystem da placa (R27) |
| `historico/` | A página de histórico das aulas, publicada no GitHub Pages em <https://caioplaninschek.github.io/presente/>; a leitura exige login (R34, R50) |
| `docs/testes/` | As páginas das rodadas na placa e o monitor do serial, publicadas no mesmo endereço, em `/testes/<nome>.html` |
| `supabase/` | O SQL do banco, em arquivos numerados. Arquivo já aplicado não se edita (R33) |
| `ferramentas/` | O script que gera as contas de professor (`senha-professor.ps1`) |
| `include/` | O `secrets.example.h`, modelo das credenciais |

## Equipe

Gabriel Santarello · Cauã Andrade · Igor Lobato · Caio Planinschek · João Victor Cabral

## Credenciais

Este repositório é público. Credenciais vivem em `include/secrets.h`, que **não** é versionado. Para rodar:

```bash
cp include/secrets.example.h include/secrets.h
# preencha os campos
```

As contas de professor ficam em `data/professores.json`, que também **não** é versionado: o `ferramentas/senha-professor.ps1` o gera, com o hash de cada senha. O que está no git é o `data/professores.example.json`.
