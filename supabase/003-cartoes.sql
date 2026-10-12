-- 003-cartoes.sql — Presente!: os cartões comprados entram no banco (tarefa 5.6, #40).
--
-- Troca o uid inventado pelo 002 pelo número de um cartão de verdade, lido pelo Igor
-- em 06/10 no comentário da #40 (R54, R65). Só altera alunos que já existem.
--
--   5 cartões -> os cinco integrantes do grupo
--   4 cartões -> alunos fictícios da turma 4172CMPN6A_P1 (1240990001 a 1240990004)
--   37A30251  -> SEM CADASTRO de propósito: é o crachá de fora da turma do teste 3 (§8)
--   62EF7E05  -> o cartão do kit, fora do banco: os testes usam só os comprados (§4)
--
-- Como aplicar: depois do 002, no SQL Editor -> New query -> colar inteiro -> Run.
-- Se algum número ou matrícula estiver errado, nada é gravado e o erro aparece.
--
-- NÃO EDITE ESTE ARQUIVO DEPOIS DE APLICADO. Mudança vira arquivo novo (004-...).

do $$
declare
  v_trocados integer;
begin
  update public.alunos a
     set uid = c.uid
    from (values
      ('1240110815', '67571251'),  -- Gabriel
      ('1240109764', '87F2FE50'),  -- Cauã
      ('1240114118', 'F78A1451'),  -- Igor
      ('1240205596', '37700951'),  -- Caio
      ('1240108001', 'B77C0951'),  -- João
      ('1240990001', '37490151'),  -- Ana Beatriz Moreira (fictícia)
      ('1240990002', 'D70B1551'),  -- Bruno Henrique Tavares (fictício)
      ('1240990003', '97100051'),  -- Camila Rocha Figueiredo (fictícia)
      ('1240990004', '77751851')   -- Daniel Augusto Pires (fictício)
    ) as c(matricula, uid)
   where a.matricula = c.matricula;
  get diagnostics v_trocados = row_count;

  -- Matrícula errada não dá erro sozinha: só deixa de trocar a linha.
  if v_trocados <> 9 then
    raise exception 'esperava trocar 9 cartoes, troquei %; nada foi gravado', v_trocados;
  end if;
end;
$$;
