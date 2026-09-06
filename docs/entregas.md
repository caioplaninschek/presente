# Entregas — enunciados do professor

Fonte: Teams da disciplina, aba Tarefas. **O cronograma válido é o revisado** (datas de terça). As datas de segunda que circulam em outra versão do slide são da outra turma.

| Data | Entrega | Estado |
|---|---|---|
| 27/08 | Formação da equipe, tema e problema | ✅ entregue |
| 02/09 | Canvas + lista de componentes + diagrama em blocos | ✅ entregue |
| **07/09 20:00** | **Projeto detalhado da solução** | 📄 **PDF pronto, falta enviar** |
| 14/09 | Projeto de desenvolvimento: programação planejada, lógica, conexões, plano de testes | ⬜ |
| 22/09 | Protótipo eletrônico: ESP32 + sensores + atuador + testes documentados | ⬜ |
| 29/09 | Estrutura física do protótipo | ⬜ |
| 05/10 | Dashboard (ESP32 obrigatório) ou interface Serial/Processing | ⬜ |
| 20/10 | Protótipo completo + vídeo de demonstração (2 minutos) | ⬜ |
| 27/10 | Exposição oficial na Semana da Computação | ⬜ |
| 30/11 | Relatório técnico completo + versão final aprimorada | ⬜ |

Requisitos obrigatórios do projeto integrador, válidos para todas as entregas: microcontrolador **ESP32**; mínimo de **2 sensores**; mínimo de **1 atuador**; programação em **C/C++**; estrutura física; documentação completa; dashboard ou interface de monitoramento. Diário de bordo semanal obrigatório.

---

## Entrega 03 — Projeto detalhado da solução

**Prazo: 07/09/2026, 20:00. Formato: PDF de 2 a 4 páginas.**

O PDF deve conter, nesta ordem:

**1. Identificação do projeto** — nome do projeto, nome dos 5 integrantes, **turma**, problema escolhido.

**2. Problema** — qual problema real o projeto pretende resolver. Explicar brevemente: quem possui esse problema; por que ele é relevante; como ele é resolvido atualmente.

**3. Solução proposta** — como o protótipo com ESP32 pretende resolver o problema. Descrever o funcionamento esperado do sistema.

**4. Componentes** — tabela no formato `Componente | Qtd | Função`.

**5. Diagrama de blocos** — o fluxo `Sensores → ESP32 → Processamento → Atuador`, mais os outros elementos necessários.

**6. Funcionamento esperado** — passo a passo, no formato *"Quando o sensor X detectar determinada condição, o ESP32 deverá…, fazendo com que o atuador…"*.

**7. Divisão das responsabilidades da equipe** — quem responde por hardware, programação, estrutura física, dashboard/interface, documentação/apresentação.

**8. Cronograma da equipe** — o que cada integrante pretende desenvolver **até 21/09**.

**Pergunta obrigatória a ser respondida:** *Qual é o principal desafio técnico que a equipe acredita que encontrará durante a construção do protótipo?*

### Onde cada seção já está resolvida

| Seção | Fonte na spec | Falta |
|---|---|---|
| 1 | cabeçalho | ⬜ **o nome/código da turma** — campo em branco no PDF |
| 2 | §1 | — |
| 3 | §1 + §3 + §6 | — |
| 4 | §4 | — |
| 5 | §3 | ✅ virado imagem (SVG no `entrega-03.html`) |
| 6 | §5 (máquina de estados) + §6 (16 passos) | — |
| 7 | §9 | — |
| 8 | §9 | — |
| Pergunta obrigatória | §10 | — |

⚠️ **Divergência a assumir explicitamente.** O Canvas da Entrega 02 prometia *"1º toque registra Entrada, 2º toque registra Saída"*. A spec retirou o controle de saída (R8). A Entrega 03 precisa de uma linha declarando a mudança e o motivo — sem isso, o professor lê como incoerência entre as entregas.
