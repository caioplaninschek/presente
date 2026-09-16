# Preparar a máquina para trabalhar no projeto

O `AGENTS.md`, na raiz do repositório, é o arquivo de contexto do projeto: ele diz onde cada coisa vive, o que fazer quando uma decisão fecha, como funciona o diário de bordo, qual é o padrão dos textos e o que nunca pode ser commitado. Quase toda ferramenta de IA de programação lê esse arquivo por conta própria, desde que a pasta aberta no editor seja a raiz do repositório.

Quem vai escrever código ou documento para o projeto precisa chegar até esse ponto antes de começar.

## 1. Git na máquina

Abra o terminal e rode `git --version`. Se não responder, instale o Git em <https://git-scm.com/download/win> com as opções padrão.

Configure uma vez, com o seu nome e o e-mail da sua conta do GitHub:

```bash
git config --global user.name "Seu Nome"
git config --global user.email "voce@exemplo.com"
git config --global pull.rebase false
```

A terceira linha evita a mensagem sobre históricos divergentes no primeiro `git pull` que encontrar trabalho de outra pessoa.

## 2. Clonar o repositório

Escolha uma pasta de caminho curto, sem acento e fora do OneDrive. O compilador do ESP32 falha em caminho com acento, e o OneDrive tenta sincronizar os milhares de arquivos temporários que cada compilação gera.

```bash
cd C:/Users/SeuUsuario/Projetos
git clone https://github.com/caioplaninschek/presente.git
cd presente
```

Na primeira vez que você der `git push`, o Git abre o navegador pedindo login no GitHub. Você é colaborador do repositório, então o acesso já está liberado.

## 3. Abrir a pasta raiz no editor

Abra a pasta `presente` inteira, não um arquivo solto nem uma subpasta: a IA só encontra o `AGENTS.md` quando ele está na raiz do que o editor tem aberto. No VS Code, **Arquivo → Abrir Pasta →** `presente`.

## 4. Confirmar que a IA está lendo o contexto

| Ferramenta | O que fazer |
|---|---|
| Codex CLI, Cursor, Antigravity | Nada. Leem o `AGENTS.md` assim que a pasta abre. |
| Claude Code | Nada. O `CLAUDE.md` da raiz manda ler o `AGENTS.md`. |
| Copilot no VS Code | Nada, em versão recente. Se ele ignorar o arquivo, ligue `chat.useAgentsMdFile` nas configurações. |
| Outra ferramenta | Comece cada sessão pedindo: "leia o `AGENTS.md` da raiz antes de responder". |

Depois, teste. Pergunte à IA: **"segundo o `AGENTS.md`, o que acontece quando uma decisão fecha numa issue?"**

A resposta tem que descrever os cinco passos na ordem: a decisão vira um `R<n>` na `docs/spec.md`, você varre quem repete a decisão em outro documento, a opção derrubada é riscada com razão e data, documento já entregue ao professor não se reescreve, e o `DIARIO.md` registra o que muda para o professor. Se vier qualquer outra coisa, ela não leu o arquivo, e o motivo quase sempre é o editor estar aberto numa pasta que não é a raiz.

## 5. `git pull` antes de começar, sempre

A regra não tem exceção. Nesta fase, a spec, os tickets de decisão e o enunciado das issues mudam quase todo dia, e código escrito sobre cópia velha é código refeito.

```bash
git pull
```

Ao terminar o trabalho:

```bash
git add .
git commit -m "mensagem em português dizendo o que mudou"
git pull
git push
```

O `git pull` antes do `git push` resolve o conflito mais comum, que é alguém ter commitado enquanto você trabalhava.

Faça commits pequenos, um por assunto. O histórico do git é a prova de evolução semanal que o professor cobra, e uma semana inteira amontoada num commit só não serve como prova.

## 6. O que ler antes de pegar uma tarefa

- **A issue** é o enunciado completo da tarefa: o passo a passo, o que tem que aparecer e o que fazer se der errado.
- **`docs/spec.md`** guarda a decisão que sustenta o critério, a pinagem do §4.1 e o contrato de dados do §7.
- **`.scratch/prototipo/graph.md`** mostra de quem a sua tarefa depende e quem está esperando por ela.

Se a IA propuser mudar arquitetura, pinagem ou contrato de dados, pare. Isso é decisão de grupo, e o caminho dela está no `AGENTS.md`.

## 7. Firmware

**Antes de instalar o PlatformIO, confira o nome da sua pasta de usuário.** Abra o Explorador de Arquivos, digite `%USERPROFILE%` na barra de endereço e veja o caminho. Se tiver acento ou espaço (por exemplo, `C:\Users\João`), crie uma pasta curta, sem acento e fora do OneDrive, como `C:\pio`. Depois crie uma variável de ambiente da sua conta chamada `PLATFORMIO_CORE_DIR`, com o caminho dessa pasta, e feche e abra o VS Code. Se instalar antes, o PlatformIO baixa tudo no caminho com acento, e o download inteiro precisa ser refeito.

Para compilar e gravar na placa, instale o **PlatformIO IDE** pela aba de extensões do VS Code. A issue #14 é a tarefa zero e detalha o resto: driver USB, identificação da porta COM e primeiro upload.

Cada programa de bancada tem a sua pasta em `src/` e o seu ambiente no `platformio.ini` (spec, R28). Para gravar um programa, escolha o ambiente dele no seletor da barra azul, na parte de baixo do VS Code (`env:ambiente`, `env:leitor`...), e só então clique em Upload. Nunca grave com `Default` selecionado: o PlatformIO grava todos os programas em sequência, e a placa fica com o último, sem aviso de erro.

Quando o firmware precisar do Wi-Fi ou do banco:

```bash
cp include/secrets.example.h include/secrets.h
```

Preencha o `include/secrets.h` e nunca o commite. O repositório é público. O `.gitignore` já bloqueia o arquivo, mas a conferência final é sua.

## 8. Se travar

Comente na issue da tarefa e cole a mensagem de erro inteira. Ninguém aqui precisa perder a noite sozinho em problema de ambiente.
