-- 004-limpeza-dos-testes.sql — Presente!: as aulas de teste saem do banco (tarefa 5.17, #51).
--
-- Apaga as aulas inventadas pelo 002 (id começando com "teste-") e as que o
-- supabase/verificar.ps1 e as bancadas deixam (id começando com "verificacao-"),
-- com os eventos delas. Elas saem antes do primeiro relatório de aula de verdade
-- (R33), e antes da rodada do pacote 4 na placa, #48 (R73).
--
-- Alunos, turmas e professores ficam como estão: o verificar.ps1 usa a turma
-- TESTE01 e o prof-teste-2, e o aparelho entra com o prof-teste-1.
--
-- Como aplicar: SQL Editor -> New query -> colar inteiro -> Run.
-- Pode rodar de novo: cada rodada do verificar.ps1 deixa outra aula "verificacao-".
--
-- NÃO EDITE ESTE ARQUIVO DEPOIS DE APLICADO. Mudança vira arquivo novo (005-...).

begin;

-- Os eventos primeiro: eventos.sessao_id aponta para sessoes.id, sem cascade.
delete from public.eventos
 where sessao_id like 'teste-%' or sessao_id like 'verificacao-%';

delete from public.sessoes
 where id like 'teste-%' or id like 'verificacao-%';

commit;
