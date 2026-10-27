# Checklist do dia da exposição, 27/10

Exposição às 18h30, no andar de baixo, no formato de feira, com a caixa na mesa (R81). Este é o checklist impresso da tarefa do dia da exposição (#64), com o pareamento do hotspot (HU-28, HU-29 e R13d da `docs/spec.md`). O que se mostra a quem passar está no roteiro da exposição (#63).

**Imprima e escreva as senhas à mão.** Senha nunca entra no repositório.

## O que levar

- [ ] A caixa fechada, com as cinco etiquetas (está com o Caio).
- [ ] O cabo USB-C de **dados**. Cabo que só carrega liga a placa, mas não mostra o monitor serial.
- [ ] O notebook e o carregador dele. Sem carregador compatível e sem power bank, a caixa liga pelo USB do notebook.
- [ ] Extensão ou benjamim, se a tomada ficar longe da mesa.
- [ ] O celular do João (Jhonez), que é o hotspot principal gravado na placa, e o Galaxy S20 FE do Caio (CaioS20FE), que é o de reserva. Os dois carregados.
- [ ] Os onze cartões etiquetados.
- [ ] A etiqueta NFC e o QR code do formulário, com o João (#74).
- [ ] Este papel, com as senhas preenchidas.

## Senhas

| Para quê | Senha |
|---|---|
| Rede do aparelho, `Presente-Sala01` | |
| `prof-teste-1` | |
| `prof-teste-2` | |
| Conta de demonstração da página de histórico | |
| Hotspot Jhonez | |
| Hotspot CaioS20FE | |

## Antes de sair de casa

- [ ] **Banco acordado (Caio, de manhã).** Abrir o painel do Supabase e o projeto do Presente!. Se aparecer *Paused*, clicar em *Restore project* e esperar uns 2 minutos. Pausado, o banco faz a busca da turma e o Enviar falharem, sem defeito nenhum na placa (R33).
- [ ] **Notebook sem dormir.** Configurações, Sistema, Energia, Tela e suspensão: *Nunca* quando ligado na tomada. Se o notebook suspender, a porta USB pode desligar, e a placa reinicia no meio da demonstração.

## Pareamento do hotspot (R13d)

A placa espera o hotspot principal por 7 s e, se ele não responder, tenta o de reserva por até 5 s. Com os dois desligados, o login cai no modo offline.

- **Android:** escolher a banda de 2,4 GHz explicitamente. A placa não enxerga 5 GHz.
- **iPhone:** ligar *Maximizar Compatibilidade* no Ponto de Acesso, que força os 2,4 GHz, e manter a tela do hotspot aberta durante o pareamento, porque o iOS suspende o sinal.
- O nome e a senha do hotspot têm de ser exatamente os gravados na placa. Celular com o nome ou a senha trocados não conecta.

## Na mesa, antes do primeiro visitante

1. Notebook na tomada e caixa no USB do notebook. O LED fica **azul fixo**: pronto para o login.
2. Ligar um dos dois hotspots, em 2,4 GHz.
3. Conectar o celular de quem opera o portal à rede `Presente-Sala01`. Se a página de login não abrir sozinha, digitar `http://192.168.4.1/` no navegador, com `http` e sem o `s`.
4. Abrir a página de histórico num celular com 4G, em `https://caioplaninschek.github.io/presente/`, e entrar com a conta de demonstração.
5. Opcional: o monitor serial no notebook, pelo `docs/testes/monitor.html` aberto no Chrome ou no Edge (*Ligar no cabo USB*, porta CH340). Mostra a quem passar o que o aparelho faz a cada toque. A porta aceita um programa por vez.
6. Uma volta de teste: login, *Iniciar*, um cartão (verde e bip), *Encerrar*.

## Se der errado

| O que aparece | O que fazer |
|---|---|
| A lista da turma não carrega com o hotspot ligado, ou o serial diz `nao achei o endereco do banco` | Banco pausado. O Caio reativa pelo celular, e espera 2 minutos. Enquanto isso, a demonstração segue no modo offline, com o *Compartilhar* no lugar do *Enviar*. |
| O hotspot não conecta | Conferir os 2,4 GHz e se o hotspot não desligou sozinho por falta de uso. Ligar o outro celular. |
| `Login recusado` com a senha certa | Usuário em minúsculas e com hífen: `prof-teste-1`. |
| `Iniciar recusado: AINDA_SINCRONIZANDO` | A busca da turma não terminou. Esperar o painel e tocar de novo. |
| `Iniciar recusado: HORA_INVALIDA` | Conferir a data e a hora do celular que fez o login. |
| O celular aparece conectado, mas não chega ao portal | Desconectar e conectar o Wi-Fi de novo. |
| A placa reiniciou sozinha | Conferir se o notebook suspendeu. A chamada volta sozinha com as presenças de antes, e o leitor volta a ler depois de um novo login (R79). |
| A página de histórico não abre | Com o hotspot ligado, o celular usa o próprio 4G: conferir se os dados móveis estão ligados. |
| A demonstração falha de vez | Mostrar as aulas já enviadas na página de histórico, as de 20/10 e as de 26/10. |
