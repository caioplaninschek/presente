# 12: Prova de vida: LED e buzzer

Type: task
Status: ready-for-human
Dono: livre (preparar) + João (rodar)
Prazo: 17/09
Entrega: —
Blocked by: 08, 04
GitHub: https://github.com/caioplaninschek/presente/issues/24
HU: HU-06, HU-18 (§1.1 da spec)

## O que entrega

As três cores do LED e o bip obedecendo a comando, com o nome da cor batendo no monitor serial. Primeiro programa que mexe nas peças. Para buzzer de dois pinos, o bip no GPIO 33 é a checagem que vale: o 11 ligou a peça no pino de 3,3 V, que fornece mais corrente que o GPIO. Bip fraco aqui volta ao 11, para o transistor (13/09).

O enunciado completo, com o passo a passo de bancada e os critérios de pronto, vive na issue do GitHub. Ver o porquê em `../graph.md`.
