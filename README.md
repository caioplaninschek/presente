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
```

**Sensor:** RC522 (leitura de UID 13,56 MHz).
**Atuadores:** LED RGB e buzzer ativo.
**Firmware:** C++ sobre Arduino Core, build com PlatformIO.

## Documentação

| Onde | O quê |
|---|---|
| [`docs/ambiente-de-trabalho.md`](docs/ambiente-de-trabalho.md) | Como preparar a máquina: clone, IA lendo o contexto do projeto, rotina de `git pull` |
| [`docs/spec.md`](docs/spec.md) | Especificação completa: decisões, arquitetura, contrato de dados, plano de testes |
| [`DIARIO.md`](DIARIO.md) | Diário de bordo semanal |
| [`.scratch/presente/map.md`](.scratch/presente/map.md) | Mapa das decisões de projeto, abertas e fechadas |
| [`.scratch/prototipo/graph.md`](.scratch/prototipo/graph.md) | Registro de execução: as 19 tarefas, o grafo de dependência e o de-para com as Issues |
| [Issues](https://github.com/caioplaninschek/presente/issues) | O fórum do grupo e o enunciado completo de cada tarefa — passo a passo, o que tem que aparecer, o que fazer se der errado |
| [`AGENTS.md`](AGENTS.md) | Convenções do repositório |

## Equipe

Gabriel Santarello · Cauã Andrade · Igor Lobato · Caio Planinschek · João Victor Cabral

## Credenciais

Este repositório é público. Credenciais vivem em `include/secrets.h`, que **não** é versionado. Para rodar:

```bash
cp include/secrets.example.h include/secrets.h
# preencha os campos
```
