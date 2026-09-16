-- 001-esquema.sql — Presente!: as seis tabelas, quem pode o quê e as duas funções do aparelho.
--
-- Decisões: docs/spec.md, R20 (tabelas), R33 (as duas funções e os privilégios),
-- R34 (leitura do histórico só com login) e R35 (chave publicável).
--
-- Como aplicar: painel do Supabase -> SQL Editor -> New query -> colar este arquivo
-- inteiro -> Run. Só num projeto vazio.
--
-- NÃO EDITE ESTE ARQUIVO DEPOIS DE APLICADO. Mudança no banco vira arquivo novo,
-- com o próximo número (003-..., 004-...). Depois de aplicar qualquer arquivo,
-- rode supabase/verificar.ps1.

-- ---------------------------------------------------------------------------
-- 1. Tabelas (R20)
-- ---------------------------------------------------------------------------

-- Identidade do professor. A senha fica no aparelho, em professores.json (Q4, R33).
create table public.professores (
  id   text primary key,
  nome text not null
);

create table public.turmas (
  codigo     text primary key,
  disciplina text not null
);

-- Um crachá ativo por aluno: trocar a tag é trocar o uid (issue #1, ponto 1).
-- Formato do aparelho: 4 ou 7 bytes em hexadecimal maiúsculo, sem separador.
create table public.alunos (
  matricula text primary key,
  nome      text not null,
  uid       text unique
            constraint alunos_uid_formato check (uid ~ '^([0-9A-F]{8}|[0-9A-F]{14})$')
);

-- Aluno que sai da turma vira ativo = false, nunca é apagado (issue #1, ponto 3).
create table public.alunos_turmas (
  matricula text    not null references public.alunos (matricula),
  turma     text    not null references public.turmas (codigo),
  ativo     boolean not null default true,
  primary key (matricula, turma)
);

-- id = sessaoId do aparelho: <turma>-<AAAAMMDD>-<HHMM> (§7, R33).
create table public.sessoes (
  id           text primary key,
  professor_id text not null references public.professores (id),
  turma        text not null references public.turmas (codigo),
  inicio       timestamptz not null,
  fim          timestamptz,
  modo         text not null
               constraint sessoes_modo check (modo in ('online', 'offline')),
  relogio      text
               constraint sessoes_relogio check (relogio in ('portal', 'api')),
  -- O aparelho só escreve 'finalizada' (R42): a sessão chega no Enviar, já
  -- encerrada, e o enviar_relatorio nomeia o estado no insert, então o default
  -- 'aberta' nunca chega a valer por esse caminho. 'aberta' e 'cancelada' são
  -- escrita administrativa, de quem corrige uma aula à mão; o 'aberta' pode
  -- voltar a ser escrito pelo aparelho quando a lacuna "estado depois do
  -- Enviar" (§8 da spec) for decidida na integração.
  estado       text not null default 'aberta'
               constraint sessoes_estado check (estado in ('aberta', 'finalizada', 'cancelada'))
);

-- Sem chave estrangeira em matricula nem em lancado_por: a matrícula digitada à mão
-- não pode recusar o relatório inteiro (R33).
create table public.eventos (
  id          bigint generated always as identity primary key,
  sessao_id   text    not null references public.sessoes (id),
  uid         text,
  matricula   text,
  horario     timestamptz not null,
  origem      text    not null
              constraint eventos_origem check (origem in ('nfc', 'manual')),
  enriquecido boolean not null default false,
  lancado_por text,
  motivo      text,
  constraint eventos_campos_por_origem check (
    (origem = 'nfc' and uid is not null)
    or (origem = 'manual' and matricula is not null and lancado_por is not null)
  )
);

-- ---------------------------------------------------------------------------
-- 2. Quem pode o quê (R33, R34)
-- ---------------------------------------------------------------------------
-- anon          = a chave publicável sem login: o aparelho. Não toca tabela nenhuma.
-- authenticated = a chave publicável com login: o painel do histórico. Só lê.

alter table public.professores   enable row level security;
alter table public.turmas        enable row level security;
alter table public.alunos        enable row level security;
alter table public.alunos_turmas enable row level security;
alter table public.sessoes       enable row level security;
alter table public.eventos       enable row level security;

revoke all on table
  public.professores, public.turmas, public.alunos,
  public.alunos_turmas, public.sessoes, public.eventos
  from anon, authenticated;

grant usage on schema public to anon, authenticated;

grant select on table
  public.professores, public.turmas, public.alunos,
  public.alunos_turmas, public.sessoes, public.eventos
  to authenticated;

create policy leitura_com_login on public.professores   for select to authenticated using (true);
create policy leitura_com_login on public.turmas        for select to authenticated using (true);
create policy leitura_com_login on public.alunos        for select to authenticated using (true);
create policy leitura_com_login on public.alunos_turmas for select to authenticated using (true);
create policy leitura_com_login on public.sessoes       for select to authenticated using (true);
create policy leitura_com_login on public.eventos       for select to authenticated using (true);

-- ---------------------------------------------------------------------------
-- 3. As duas funções do aparelho (R33)
-- ---------------------------------------------------------------------------

-- No login: a lista de UMA turma (matrícula, nome e uid dos ativos).
-- Chamada: POST /rest/v1/rpc/roster  com corpo {"turma": "4172CMPN6A_P1"}
create function public.roster(turma text)
returns table (matricula text, nome text, uid text)
language sql
stable
security definer
set search_path = ''
as $$
  select a.matricula, a.nome, a.uid
    from public.alunos_turmas mt
    join public.alunos a on a.matricula = mt.matricula
   where mt.turma = roster.turma
     and mt.ativo
   order by a.nome;
$$;

-- No Enviar: o relatório inteiro, no mesmo JSON do Compartilhar (§7), numa transação.
-- Chamada: POST /rest/v1/rpc/enviar_relatorio  com corpo {"relatorio": <JSON do §7>}
-- Respostas:
--   {"resultado": "gravado", "eventos": n, "enriquecidos_no_banco": m}
--   {"resultado": "ja_enviado"}  quando a sessão já chegou antes: nada é gravado (HU-39)
create function public.enviar_relatorio(relatorio jsonb)
returns jsonb
language plpgsql
security definer
set search_path = ''
as $$
declare
  v_sessao       jsonb := relatorio -> 'sessao';
  v_id           text  := relatorio -> 'sessao' ->> 'sessaoId';
  v_eventos      integer;
  v_enriquecidos integer;
begin
  if v_id is null or v_id = '' then
    raise exception 'relatorio sem sessao.sessaoId' using errcode = '22023';
  end if;

  -- A sessão só chega ao banco no Enviar, e já finalizada (R33).
  insert into public.sessoes (id, professor_id, turma, inicio, fim, modo, relogio, estado)
  values (
    v_id,
    v_sessao ->> 'professorId',
    v_sessao ->> 'turma',
    (v_sessao ->> 'inicio')::timestamptz,
    (v_sessao ->> 'fim')::timestamptz,
    v_sessao ->> 'modo',
    v_sessao ->> 'relogio',
    'finalizada'
  )
  on conflict (id) do nothing;

  if not found then
    return jsonb_build_object('resultado', 'ja_enviado');
  end if;

  -- enriquecido guarda o que o aparelho mandou (R33).
  insert into public.eventos (sessao_id, uid, matricula, horario, origem, enriquecido, lancado_por, motivo)
  select v_id, e.uid, e.matricula, e."timestamp", e.origem,
         coalesce(e.enriquecido, false), e.lancado_por, e.motivo
    from jsonb_to_recordset(coalesce(relatorio -> 'eventos', '[]'::jsonb))
      as e(uid text, matricula text, "timestamp" timestamptz, origem text,
           enriquecido boolean, lancado_por text, motivo text);
  get diagnostics v_eventos = row_count;

  -- Modo offline: o banco cruza UID -> matrícula na chegada (R3, issue #1 ponto 6).
  update public.eventos ev
     set matricula = a.matricula
    from public.alunos a
   where ev.sessao_id = v_id
     and ev.matricula is null
     and ev.uid = a.uid;
  get diagnostics v_enriquecidos = row_count;

  return jsonb_build_object(
    'resultado', 'gravado',
    'eventos', v_eventos,
    'enriquecidos_no_banco', v_enriquecidos
  );
end;
$$;

revoke all on function public.roster(text)            from public, anon, authenticated;
revoke all on function public.enviar_relatorio(jsonb) from public, anon, authenticated;
grant execute on function public.roster(text)            to anon;
grant execute on function public.enviar_relatorio(jsonb) to anon;

-- Faz a API do Supabase enxergar as funções novas sem esperar.
notify pgrst, 'reload schema';
