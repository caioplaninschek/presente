-- 002-dados-de-teste.sql — Presente!: alunos, turmas e aulas passadas de mentira,
-- para a tela de histórico parecer um sistema (issue #21).
--
-- Como aplicar: depois do 001, no SQL Editor -> New query -> colar inteiro -> Run.
--
-- O que é de verdade aqui: só o nome e a matrícula dos cinco integrantes do grupo,
-- que já estão públicos no cabeçalho de docs/spec.md. Todo o resto é inventado.
-- Os UIDs são inventados (derivados da matrícula); os das tags reais entram depois,
-- num 003-uids-reais.sql.
--
-- As aulas passadas têm id começando com "teste-" e saem na integração com o
-- aparelho (epic #9), antes do primeiro relatório de aula real.
--
-- NÃO EDITE ESTE ARQUIVO DEPOIS DE APLICADO. Mudança vira arquivo novo (003-...).

-- 1. Professores fictícios. Nunca o professor real (aulas inventadas).
insert into public.professores (id, nome) values
  ('prof-teste-1', 'Prof.ª Regina Falcão (teste)'),
  ('prof-teste-2', 'Prof. Marcos Bezerra (teste)');

-- 2. Turmas: a real e uma fictícia.
insert into public.turmas (codigo, disciplina) values
  ('4172CMPN6A_P1', 'Sistemas Embarcados'),
  ('TESTE01', 'Disciplina de Teste');

-- 3. Alunos. UID inventado e fixo: os primeiros 8 caracteres do md5 da matrícula
-- (4 bytes); matrícula terminada em 3 ou 7 ganha 14 caracteres (7 bytes, NTAG215).
insert into public.alunos (matricula, nome, uid)
select v.matricula, v.nome,
       upper(substr(md5(v.matricula), 1,
             case when right(v.matricula, 1) in ('3', '7') then 14 else 8 end))
  from (values
    -- o grupo (matrícula real, pública na spec)
    ('1240110815', 'Gabriel Albuquerque Varela Santarello'),
    ('1240109764', 'Cauã Manuel Proença de Andrade'),
    ('1240114118', 'Igor Rocha Lobato'),
    ('1240205596', 'Caio Parada Oliveira Planinschek'),
    ('1240108001', 'João Victor Berçot Chabudet Cabral'),
    -- fictícios da turma real (1240990001 a 1240990005 também estão na TESTE01)
    ('1240990001', 'Ana Beatriz Moreira'),
    ('1240990002', 'Bruno Henrique Tavares'),
    ('1240990003', 'Camila Rocha Figueiredo'),
    ('1240990004', 'Daniel Augusto Pires'),
    ('1240990005', 'Eduarda Lima Castanheira'),
    ('1240990006', 'Felipe Nogueira Barros'),
    ('1240990007', 'Gabriela Antunes Reis'),
    ('1240990008', 'Henrique Vasconcelos Dias'),
    ('1240990009', 'Isabela Cardoso Mendes'),
    ('1240990010', 'Júlio Rezende Farias'),
    ('1240990011', 'Larissa Monteiro Alves'),
    ('1240990012', 'Lucas Gabriel Esteves'),
    ('1240990013', 'Mariana Teixeira Lobo'),
    ('1240990014', 'Mateus Correia Brandão'),
    ('1240990015', 'Natália Prado Siqueira'),
    ('1240990016', 'Otávio Ribeiro Salles'),
    ('1240990017', 'Paula Fernandes Aguiar'),
    ('1240990018', 'Pedro Henrique Couto'),
    ('1240990019', 'Rafaela Gusmão Leal'),
    ('1240990020', 'Renan Oliveira Queiroz'),
    ('1240990021', 'Sabrina Duarte Vilela'),
    ('1240990022', 'Samuel Batista Coelho'),
    ('1240990023', 'Tainá Moura Bittencourt'),
    ('1240990024', 'Tomás Martins Freire'),
    ('1240990025', 'Valentina Campos Rangel'),
    ('1240990026', 'Vinícius Peixoto Lacerda'),
    ('1240990027', 'Yasmin Carvalho Serra'),
    ('1240990028', 'André Luiz Magalhães'),
    ('1240990029', 'Beatriz Sampaio Cunha'),
    ('1240990030', 'Diego Vitor Almeida'),
    ('1240990031', 'Clara Mendonça Viana'),
    ('1240990032', 'Enzo Lucca Pacheco'),
    ('1240990033', 'Fernanda Holanda Porto'),
    ('1240990034', 'Gustavo Amaral Sena'),
    ('1240990035', 'Helena Barreto Quintela'),
    -- saiu da turma real: fica inativo, nunca é apagado
    ('1240990036', 'Leonardo Nascimento Lustosa'),
    -- só da TESTE01
    ('1240990101', 'Lívia Arruda Cordeiro'),
    ('1240990102', 'Marcelo Rios Toledo'),
    ('1240990103', 'Nicole Barbosa Fontes'),
    ('1240990104', 'Rodrigo Pimentel Lage'),
    ('1240990105', 'Sofia Galvão Neves'),
    ('1240990106', 'Wesley Cavalcanti Bueno'),
    ('1240990107', 'Yuri Medeiros Assunção')
  ) as v(matricula, nome);

-- 4. Quem está em qual turma.
-- Turma real: 40 ativos (o grupo + 1240990001 a 1240990035) e 1 inativo (1240990036).
insert into public.alunos_turmas (matricula, turma, ativo)
select matricula, '4172CMPN6A_P1', matricula <> '1240990036'
  from public.alunos
 where matricula not like '12409901%';

-- TESTE01: 12 alunos, 5 deles também da turma real.
insert into public.alunos_turmas (matricula, turma, ativo)
select matricula, 'TESTE01', true
  from public.alunos
 where matricula between '1240990001' and '1240990005'
    or matricula like '12409901%';

-- 5. Aulas passadas da turma real, nas terças de 04/08 a 08/09, às 19h.
-- 18/08 foi sem internet (offline); 01/09 foi cancelada.
-- A aula cancelada é semeada à mão: nenhum aparelho a produziria, porque ele só
-- escreve 'finalizada' (R42). Ela existe para a página de histórico (R21) ter os
-- três estados para mostrar, e é por isso que não recebe evento nenhum nos
-- blocos 6 e 7 abaixo, que filtram por estado = 'finalizada'.
insert into public.sessoes (id, professor_id, turma, inicio, fim, modo, relogio, estado) values
  ('teste-4172CMPN6A_P1-20260804-1900', 'prof-teste-1', '4172CMPN6A_P1', '2026-08-04 19:00:00-03', '2026-08-04 21:40:00-03', 'online',  'portal', 'finalizada'),
  ('teste-4172CMPN6A_P1-20260811-1900', 'prof-teste-1', '4172CMPN6A_P1', '2026-08-11 19:00:00-03', '2026-08-11 21:40:00-03', 'online',  'portal', 'finalizada'),
  ('teste-4172CMPN6A_P1-20260818-1900', 'prof-teste-1', '4172CMPN6A_P1', '2026-08-18 19:00:00-03', '2026-08-18 21:40:00-03', 'offline', 'portal', 'finalizada'),
  ('teste-4172CMPN6A_P1-20260825-1900', 'prof-teste-1', '4172CMPN6A_P1', '2026-08-25 19:00:00-03', '2026-08-25 21:40:00-03', 'online',  'portal', 'finalizada'),
  ('teste-4172CMPN6A_P1-20260901-1900', 'prof-teste-1', '4172CMPN6A_P1', '2026-09-01 19:00:00-03', null,                     'online',  'portal', 'cancelada'),
  ('teste-4172CMPN6A_P1-20260908-1900', 'prof-teste-1', '4172CMPN6A_P1', '2026-09-08 19:00:00-03', '2026-09-08 21:40:00-03', 'online',  'portal', 'finalizada');

-- 6. Presenças por crachá. Sorteio fixo, sem random(): cada par (aula, aluno) tem um
-- número de 0 a 99 tirado do md5; abaixo de 78 = presente. Dá o mesmo resultado
-- em qualquer banco. Na aula offline o aparelho mandou cru (enriquecido = false) e
-- o banco preencheu a matrícula na chegada.
insert into public.eventos (sessao_id, uid, matricula, horario, origem, enriquecido)
select s.id, a.uid, a.matricula,
       s.inicio + make_interval(secs => 60 + (h.n % 1500)),
       'nfc',
       s.modo = 'online'
  from public.sessoes s
  join public.alunos_turmas mt on mt.turma = s.turma and mt.ativo
  join public.alunos a on a.matricula = mt.matricula
  cross join lateral (
    select ('x' || substr(md5(s.id || a.matricula), 1, 8))::bit(32)::bigint as n
  ) h
 where s.id like 'teste-%'
   and s.estado = 'finalizada'
   and h.n % 100 < 78;

-- 7. Lançamentos manuais: 1 ou 2 por aula, entre os que não encostaram o crachá.
insert into public.eventos (sessao_id, matricula, horario, origem, lancado_por, motivo)
select f.id, f.matricula, f.inicio + make_interval(mins => (40 + f.r)::int), 'manual', 'prof-teste-1', 'esqueceu a tag'
  from (
    select s.id, s.inicio, a.matricula,
           row_number() over (partition by s.id order by md5(s.id || a.matricula)) as r,
           1 + extract(day from s.inicio at time zone 'America/Sao_Paulo')::int % 2 as quantos
      from public.sessoes s
      join public.alunos_turmas mt on mt.turma = s.turma and mt.ativo
      join public.alunos a on a.matricula = mt.matricula
     where s.id like 'teste-%'
       and s.estado = 'finalizada'
       and ('x' || substr(md5(s.id || a.matricula), 1, 8))::bit(32)::bigint % 100 >= 78
  ) f
 where f.r <= f.quantos;
