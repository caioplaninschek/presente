# 7 CONSIDERAÇÕES FINAIS

O Presente! chegou à entrega final com o escopo que o Caio fixou em 23/09: o núcleo completo, com os cortes declarados. As cinco partes do firmware rodaram na placa, e a rodada completa de testes rodou duas vezes, na aula de 20/10 e em 26/10, a segunda já com a caixa pintada e fechada; nas duas, os testes passaram, com as exceções que a seção dos testes declara, e parte do resultado existe só pelo relato do grupo, em 20/10, e pelo do Caio, em 26/10. Em 27/10, mostramos o aparelho na exposição da Semana da Computação, no formato de feira, com a caixa na mesa, chamadas de demonstração abertas para os visitantes e o vídeo de apresentação do projeto, feito pelo Cauã, no notebook. Pelo relato do Caio, a demonstração correu sem falhas, e o professor e a reitora a acompanharam.

A forma do aparelho veio de duas decisões de arquitetura. Manter a rede do aparelho no ar enquanto ele usa o hotspot tirou do projeto a troca de rádio e o risco de a rede do aparelho não voltar, e concentrar a internet no login e no envio deixou cada toque de crachá resolvido dentro do aparelho. Gravar cada presença antes do sinal garante que nenhuma presença confirmada ao aluno se perca numa queda de energia; a suspeita, não confirmada, é que essa gravação esteja na origem dos quatro toques acima de 200 ms, que o Caio decidiu não corrigir a uma semana da exposição. Como caminhos de evolução, ficam registrados o celular do aluno no lugar do crachá e o celular do professor falando com o banco no lugar do ESP32, que descartamos por razão acadêmica; em produção, a instituição forneceria uma rede própria para os aparelhos, e o sistema acadêmico real tomaria o lugar do banco que faz esse papel no protótipo.

Cada integrante respondeu por partes diferentes do projeto, e a montagem da caixa, em 05/10, e a exposição, em 27/10, reuniram os cinco (Quadro 11).

**Quadro 11: Quem fez cada parte do projeto**

| Integrante | Responsável por |
|----|----|
| Caio | A especificação e a coordenação das tarefas; o banco no Supabase; os pacotes 1 a 4 do firmware, do portal ao relatório (#41, #43, #45 e #47), e a revisão do pacote 5, a queda de energia (#49); a ideia, o desenho e a pintura da caixa, e as etiquetas; os manuais dos testes e o checklist da exposição; a revisão final e o PDF deste relatório, e o PDF da entrega |
| Cauã | Os programas de teste de setembro; o pacote 5 do firmware (#49) e a revisão dos pacotes 1 a 4; o teste com a caixa fechada e o vídeo da Entrega 06; o esqueleto deste relatório e as seções do firmware e das limitações; o vídeo de apresentação do projeto, exibido na exposição de 27/10 |
| Gabriel | O fluxograma da lógica; o documento da Entrega 06; neste relatório, a introdução e a arquitetura, a estrutura física, os testes e os resultados e a consolidação |
| Igor | As telas do portal do professor e a página de histórico; o modelo da caixa no Tinkercad; a página do monitor serial; o capítulo do dashboard |
| João | As peças e as compras; a correção do modo de rádio; os testes na placa de setembro e as rodadas dos pacotes 1 a 3, em casa (#42, #44 e #46); o formulário da exposição (#74) |
| Caio, João e Igor | A rodada completa de testes de 26/10, com a caixa pintada |
| Os cinco | A montagem da caixa e o primeiro teste com ela fechada, em 05/10, e a exposição de 27/10, na Semana da Computação |

Fonte: elaborado pelos autores (2026).

# REFERÊNCIAS

ESPRESSIF SYSTEMS. Wi-Fi driver. *In*: ESPRESSIF SYSTEMS. **ESP-IDF programming guide**. Versão 4.4. [*S. l.*], [2022]. Disponível em: https://docs.espressif.com/projects/esp-idf/en/v4.4/esp32/api-guides/wifi.html. Acesso em: 10 set. 2026.

ESPRESSIF SYSTEMS. **ESP32-WROOM-32D & ESP32-WROOM-32U datasheet**. Versão 2.7. [*S. l.*], 2026. 49 p. Disponível em: https://espressif.com/documentation/esp32-wroom-32d_esp32-wroom-32u_datasheet_en.pdf. Acesso em: 10 set. 2026.

NXP SEMICONDUCTORS. **MFRC522**: standard performance MIFARE and NTAG frontend. Rev. 3.9. [*S. l.*], 27 abr. 2016. 95 p. Product data sheet. Disponível em: https://www.nxp.com/docs/en/data-sheet/MFRC522.pdf. Acesso em: 10 set. 2026.
