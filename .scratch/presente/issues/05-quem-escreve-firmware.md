# Quem escreve firmware, e como o time evita conflito no repo

Type: grilling
Status: open
Parent: map.md
Blocked by: 01

## Question

O Caio respondeu que, por ele, "é liberado para todo mundo fazer qualquer coisa no projeto, contanto que ajudem" — mas ressalvou que pode perguntar ao grupo se for necessário. É necessário: a resposta muda o fluxo de git do projeto inteiro.

O conflito a resolver: **política aberta é boa para um time de 5 que quer aprender; é ruim para um `main.cpp` de firmware embarcado.** Cinco pessoas editando o mesmo arquivo grande de C++ em paralelo geram conflito de merge quase garantido, e conflito de merge em firmware é onde iniciante trava e desiste.

Perguntas que a sessão precisa fechar:

1. **Quantos, de fato, querem escrever C++?** "Liberado para todos" e "todos vão querer" são coisas diferentes. Isso vem do grupo, não do Caio.
2. **Fluxo de git:** direto na `main` (rápido, sem cerimônia, quebra fácil) ou branch por frente + PR (mais lento, mas o histórico vira o diário de bordo que o professor cobra)?
3. **Revisão:** alguém revisa antes de entrar na `main`, ou confia-se no build do PlatformIO passando?
4. **Como a modularização já decidida ajuda.** A spec (R9, §5) já quebra o firmware em `rfid.cpp`, `storage.cpp`, `portal.cpp`, `net.cpp`, `tamper.cpp`, `feedback.cpp`. Se cada dono ficar no seu arquivo, o conflito quase some — mas isso só funciona se a divisão de responsabilidades estiver de pé.

## Notas

Bloqueado por **Divisão de responsabilidades confirmada com o grupo**: não dá para decidir quem trabalha em qual arquivo antes de saber quem é dono de qual frente.

Recomendação já registrada e a ser confrontada na sessão: 2 donos no firmware, os outros nas suas camadas. O Caio ainda não concordou nem discordou disso — respondeu a política, não a mecânica.
