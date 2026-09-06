# Lista final de compras e quem compra

Type: task
Status: open
Parent: map.md
Blocked by: —
Prazo real: peças na mão até 12–14/09

## Question

A spec lista como pendentes: LED RGB catodo comum, 3× resistor 220 Ω, buzzer ativo 5V, jumpers M-M e M-F, fonte 5V 2A. Mas essa lista é inferida do que a spec diz que já foi adquirido — ninguém do grupo confirmou o inventário real.

O Caio vai cobrar do João o levantamento do que falta. Este ticket fecha quando existir:

1. **Inventário real** do que o grupo já tem em mãos (quem está com o quê, fisicamente).
2. **Lista de compra** final, com quantidades.
3. **Quem compra e quando**, com a restrição de prazo: a integração sensor + atuador é entregável de **22/09**, então as peças precisam estar na mão por volta de **12–14/09**.
4. **Loja física ou online.** Recomendação registrada: física, compra única, uma pessoa só — pelo prazo, frete de 5 a 10 dias é risco desnecessário para peças de R$5 a R$20.

Item de atenção: o **buzzer**. Se o modelo comprado for buzzer ativo 5V sem driver, ele não liga direto no GPIO do ESP32 e vai precisar de um transistor 2N2222 + resistor de base. Comprar o transistor junto sai mais barato que uma segunda ida à loja.

## Notas

`task` porque não há decisão de engenharia aqui — há trabalho manual (levantar o inventário, comprar) que bloqueia o cronograma de 22/09. Resolvido quando a compra estiver feita ou com data marcada e responsável definido.
