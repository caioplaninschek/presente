# CAPA

- UNIVERSIDADE VEIGA DE ALMEIDA
- Bacharelado em Ciência da Computação
- GABRIEL ALBUQUERQUE VARELA SANTARELLO (1240110815)
- CAUÃ MANUEL PROENÇA DE ANDRADE (1240109764)
- IGOR ROCHA LOBATO (1240114118)
- CAIO PARADA OLIVEIRA PLANINSCHEK (1240205596)
- JOÃO VICTOR BERÇOT CHABUDET CABRAL (1240108001)
- **PRESENTE!**
- DISPOSITIVO EMBARCADO PARA CONTABILIZAÇÃO DE PRESENÇA EM SALA DE AULA
- (Trabalho da disciplina de Sistemas Embarcados)
- Turma 4172CMPN6A_P1
- **Relatório técnico, documento complementar da entrega final**
- Professor: Thiago Alberto Ramos Gabriel
- Rio de Janeiro, RJ
- 2026

# RESUMO

Este relatório técnico apresenta o Presente!, terminal embarcado que registra a presença do aluno em sala de aula pela aproximação do crachá, desenvolvido na disciplina de Sistemas Embarcados para substituir a chamada manual, que consome de 10 a 15 minutos de cada aula. O aparelho usa o microcontrolador ESP32, o leitor RFID RC522, um LED RGB e um buzzer ativo, dentro de uma caixa de tênis de papelão reaproveitada e pintada de prata. O professor o opera pelo navegador do celular, sem instalar aplicativo, na rede Wi-Fi que o próprio aparelho põe no ar. A internet, de um hotspot de celular, só é usada no login, para buscar a lista da turma, e no envio do relatório a um banco PostgreSQL no Supabase, que faz no protótipo o papel do sistema acadêmico; uma página de histórico mostra os presentes e os faltantes de cada aula a quem entra com e-mail e senha. Sem internet, o aparelho segue no modo offline, e, depois de uma queda de energia, a chamada volta com o que já estava registrado. Nos testes na placa e na exposição, de 21/09 a 27/10, o aparelho cumpriu os critérios do plano com duas exceções declaradas: quatro toques passaram dos 200 ms pedidos entre o crachá e o sinal, dois em 20/10, um em 26/10 e um em 27/10, e o painel do professor durante a troca de canal da rede não pôde ser observado nas rodadas de 20/10. O que não rodou ou ficou sem registro está na seção 5.3. Parte da evidência de 22/09 e de 20/10 existe só pelo relato do grupo, e a de 25 e 26/10, só pelo do Caio. O aparelho foi demonstrado na exposição da Semana da Computação, em 27/10, e, pelo relato do Caio, a demonstração correu sem falhas, e o professor e a reitora a acompanharam. As limitações aceitas estão declaradas com a razão de cada uma.

**Palavras-chave:** registro de presença; ESP32; RFID; portal cativo; banco de dados na nuvem.
