'use strict';
/* Presente! — página de histórico das aulas.
 *
 * Lê o banco direto, pelas rotas REST do Supabase, e não conversa com o
 * aparelho: o que está aqui é o que já foi enviado e gravado. Só leitura — as
 * políticas do banco dão ao papel `authenticated` o select das seis tabelas e
 * mais nada (R33, R34).
 *
 * Sem biblioteca: `fetch` basta para as três chamadas que a página faz, e uma
 * dependência de CDN seria uma coisa a mais para sair do ar no dia da
 * apresentação.
 */
(function () {

  var CONFIG = window.CONFIG || {};
  var API = String(CONFIG.url || '').replace(/\/+$/, '');
  var CHAVE = CONFIG.chave || '';

  // O token do login vive na aba, e morre com ela. Guardá-lo é o que evita
  // pedir a senha de novo a cada recarga; não guardá-lo em `localStorage` é o
  // que evita que ele sobreviva ao fechar do navegador.
  var GUARDA = 'presente.historico.sessao';

  var token = null;
  var alunosDaTurma = {};        // turma -> [{ matricula, nome }], uma vez por turma

  /* ------------------------------------------------------------ atalhos -- */

  function $(id) { return document.getElementById(id); }

  function texto(tag, classe, conteudo) {
    var el = document.createElement(tag);
    if (classe) el.className = classe;
    if (conteudo !== undefined && conteudo !== null) el.textContent = conteudo;
    return el;
  }

  function erro(id, mensagem) {
    var el = $(id);
    if (!mensagem) { el.hidden = true; el.textContent = ''; return; }
    el.textContent = mensagem;
    el.hidden = false;
  }

  var TELAS = ['login', 'aulas', 'aula'];

  function mostrar(nome) {
    TELAS.forEach(function (t) { $('tela-' + t).hidden = (t !== nome); });
    $('acao-sair').hidden = (nome === 'login');
    var titulo = $('tela-' + nome).querySelector('h2');
    if (titulo) { titulo.tabIndex = -1; titulo.focus(); }
    window.scrollTo(0, 0);
  }

  /* --------------------------------------------------------- datas ------ */

  // O banco guarda `timestamptz`; a aula aconteceu no horário de Brasília, e é
  // nele que ela tem que ser lida, mesmo que o navegador esteja noutro fuso.
  var FUSO = 'America/Sao_Paulo';

  function formatar(iso, opcoes) {
    if (!iso) return '';
    var d = new Date(iso);
    if (isNaN(d.getTime())) return '';
    opcoes.timeZone = FUSO;
    try { return new Intl.DateTimeFormat('pt-BR', opcoes).format(d); }
    catch (e) { return iso.slice(0, 16).replace('T', ' '); }
  }

  function dataLonga(iso) {
    return formatar(iso, { day: '2-digit', month: '2-digit', year: 'numeric' });
  }

  function horaCurta(iso) {
    return formatar(iso, { hour: '2-digit', minute: '2-digit' });
  }

  /* ----------------------------------------------------- banco de dados -- */

  function cabecalhos() {
    // A chave publicável viaja no `apikey`, nunca em `Authorization` (R35). O
    // `Bearer` é do token do login, que é o que diz ao banco que quem lê é um
    // usuário autenticado.
    var h = { apikey: CHAVE, 'Content-Type': 'application/json' };
    if (token) h.Authorization = 'Bearer ' + token;
    return h;
  }

  function ler(caminho) {
    return fetch(API + '/rest/v1/' + caminho, { headers: cabecalhos() })
      .then(function (r) {
        // Token vencido ou revogado: a página volta ao login em vez de mostrar
        // uma lista vazia como se não houvesse aula nenhuma.
        if (r.status === 401 || r.status === 403) { sair(); throw new Error('SEM_LOGIN'); }
        if (!r.ok) throw new Error('HTTP_' + r.status);
        return r.json();
      });
  }

  /* -------------------------------------------------------- 1. LOGIN ---- */

  $('form-login').addEventListener('submit', function (ev) {
    ev.preventDefault();
    var email = $('email').value.trim();
    var senha = $('senha').value;

    if (!API || !CHAVE) {
      erro('erro-login', 'A página não foi publicada com a configuração do banco. ' +
                         'Avise o grupo: falta o config.js.');
      return;
    }
    if (!email || !senha) { erro('erro-login', 'Preencha e-mail e senha.'); return; }

    erro('erro-login', '');
    $('botao-entrar').disabled = true;

    fetch(API + '/auth/v1/token?grant_type=password', {
      method: 'POST',
      headers: { apikey: CHAVE, 'Content-Type': 'application/json' },
      body: JSON.stringify({ email: email, password: senha })
    }).then(function (r) {
      return r.json().then(function (corpo) { return { ok: r.ok, corpo: corpo }; });
    }).then(function (r) {
      $('botao-entrar').disabled = false;
      if (!r.ok || !r.corpo.access_token) {
        erro('erro-login', 'E-mail ou senha não conferem.');
        $('senha').value = '';
        $('senha').focus();
        return;
      }
      token = r.corpo.access_token;
      try { sessionStorage.setItem(GUARDA, token); } catch (e) { /* aba anônima */ }
      // A senha sai da memória da página assim que deixa de ser necessária.
      $('senha').value = '';
      carregarAulas();
    }, function () {
      $('botao-entrar').disabled = false;
      erro('erro-login', 'Não foi possível falar com o banco. Verifique a internet e tente de novo.');
    });
  });

  function sair() {
    token = null;
    try { sessionStorage.removeItem(GUARDA); } catch (e) { /* aba anônima */ }
    alunosDaTurma = {};
    $('email').value = '';
    $('senha').value = '';
    $('lista-aulas').innerHTML = '';
    erro('erro-aulas', '');
    erro('erro-aula', '');
    mostrar('login');
  }

  $('acao-sair').addEventListener('click', sair);

  /* -------------------------------------------------------- 2. AULAS ---- */

  function carregarAulas() {
    mostrar('aulas');
    erro('erro-aulas', '');
    $('aulas-resumo').textContent = 'Carregando…';
    $('vazio-aulas').hidden = true;
    $('lista-aulas').innerHTML = '';

    // O professor e a disciplina vêm pelas chaves estrangeiras, numa viagem só.
    var campos = 'id,inicio,fim,modo,estado,turma,professor_id,' +
                 'professores(nome),turmas(disciplina)';
    ler('sessoes?select=' + encodeURIComponent(campos) + '&order=inicio.desc')
      .then(function (sessoes) {
        $('aulas-resumo').textContent = sessoes.length === 1
          ? '1 aula registrada'
          : sessoes.length + ' aulas registradas';
        $('vazio-aulas').hidden = sessoes.length > 0;
        sessoes.forEach(function (s) { $('lista-aulas').appendChild(cartaoDaAula(s)); });
      })
      .catch(function (e) {
        if (e && e.message === 'SEM_LOGIN') return;
        $('aulas-resumo').textContent = '';
        erro('erro-aulas', 'Não foi possível ler as aulas do banco.');
      });
  }

  function cartaoDaAula(s) {
    var li = document.createElement('li');
    var botao = document.createElement('button');
    botao.type = 'button';
    botao.className = 'cartao-aula';

    botao.appendChild(texto('span', 'data', dataLonga(s.inicio) + ' · ' + horaCurta(s.inicio)));
    botao.appendChild(texto('span', 'quem',
      (s.professores && s.professores.nome ? s.professores.nome : s.professor_id)));
    botao.appendChild(texto('span', 'turma',
      s.turma + (s.turmas && s.turmas.disciplina ? ' · ' + s.turmas.disciplina : '')));

    var pilulas = texto('span', 'pilulas');
    pilulas.appendChild(pilulaDoEstado(s.estado));
    pilulas.appendChild(pilulaDoModo(s.modo));
    botao.appendChild(pilulas);

    botao.addEventListener('click', function () { abrirAula(s); });
    li.appendChild(botao);
    return li;
  }

  // Os três estados aparecem (R42). O aparelho só escreve `finalizada`: as
  // outras duas são correção feita à mão, e precisam se distinguir na tela,
  // senão uma aula cancelada parece uma aula sem presença nenhuma.
  function pilulaDoEstado(estado) {
    var rotulos = { aberta: 'Aberta', finalizada: 'Finalizada', cancelada: 'Cancelada' };
    return texto('span', 'pilula estado-' + estado, rotulos[estado] || estado);
  }

  function pilulaDoModo(modo) {
    return modo === 'offline'
      ? texto('span', 'pilula atencao', 'Sem internet')
      : texto('span', 'pilula', 'Com internet');
  }

  /* ----------------------------------------------------- 3. UMA AULA ---- */

  $('acao-voltar').addEventListener('click', function () { mostrar('aulas'); });

  function abrirAula(s) {
    mostrar('aula');
    erro('erro-aula', '');
    $('titulo-aula').textContent = dataLonga(s.inicio) + ' · ' + horaCurta(s.inicio);

    var cab = [];
    if (s.professores && s.professores.nome) cab.push(s.professores.nome);
    if (s.turmas && s.turmas.disciplina) cab.push(s.turmas.disciplina);
    cab.push(s.turma);
    if (s.fim) cab.push('fim ' + horaCurta(s.fim));
    $('aula-cabecalho').textContent = cab.join(' · ');

    var pilulas = $('aula-pilulas');
    pilulas.innerHTML = '';
    pilulas.appendChild(pilulaDoEstado(s.estado));
    pilulas.appendChild(pilulaDoModo(s.modo));

    limparListas('Carregando…');

    Promise.all([eventosDa(s.id), turmaDe(s.turma)])
      .then(function (r) { desenharAula(r[0], r[1]); })
      .catch(function (e) {
        if (e && e.message === 'SEM_LOGIN') return;
        limparListas('');
        erro('erro-aula', 'Não foi possível ler as presenças desta aula.');
      });
  }

  function limparListas(aviso) {
    $('lista-presentes').innerHTML = '';
    $('lista-faltantes').innerHTML = '';
    $('conta-presentes').textContent = '';
    $('conta-faltantes').textContent = '';
    $('vazio-presentes').textContent = aviso || 'Ninguém registrado nesta aula.';
    $('vazio-presentes').hidden = !aviso;
    $('vazio-faltantes').hidden = true;
  }

  function eventosDa(sessaoId) {
    var campos = 'matricula,uid,horario,origem,lancado_por,motivo';
    return ler('eventos?sessao_id=eq.' + encodeURIComponent(sessaoId) +
               '&select=' + campos + '&order=horario.asc');
  }

  // A lista da turma é a mesma para todas as aulas dela: buscar uma vez e
  // guardar evita uma viagem a cada aula aberta.
  function turmaDe(turma) {
    if (alunosDaTurma[turma]) return Promise.resolve(alunosDaTurma[turma]);
    var campos = 'matricula,alunos(nome)';
    return ler('alunos_turmas?turma=eq.' + encodeURIComponent(turma) +
               '&ativo=is.true&select=' + encodeURIComponent(campos))
      .then(function (linhas) {
        var lista = linhas.map(function (l) {
          return { matricula: l.matricula, nome: (l.alunos && l.alunos.nome) || l.matricula };
        });
        lista.sort(function (a, b) { return a.nome.localeCompare(b.nome, 'pt-BR'); });
        alunosDaTurma[turma] = lista;
        return lista;
      });
  }

  function desenharAula(eventos, turma) {
    limparListas('');

    var nomePor = {};
    turma.forEach(function (a) { nomePor[a.matricula] = a.nome; });

    // Cada matrícula conta uma vez, pelo primeiro evento dela: o aparelho já
    // recusa o segundo toque na mesma chamada, mas a aula corrigida à mão pode
    // ter duas linhas para a mesma pessoa, e o histórico não pode duplicá-la.
    var vistos = {};
    var presentes = [];
    var semNome = [];

    eventos.forEach(function (ev) {
      if (!ev.matricula) {
        // Aula sem internet: o aparelho gravou o número do crachá, e o nome só
        // existiria se a matrícula tivesse sido resolvida no envio (R3).
        semNome.push(ev);
        return;
      }
      if (vistos[ev.matricula]) return;
      vistos[ev.matricula] = true;
      presentes.push(ev);
    });

    $('conta-presentes').textContent = '(' + (presentes.length + semNome.length) + ')';
    presentes.forEach(function (ev) {
      $('lista-presentes').appendChild(linhaPresente(ev, nomePor[ev.matricula]));
    });
    semNome.forEach(function (ev) {
      $('lista-presentes').appendChild(linhaSemNome(ev));
    });
    $('vazio-presentes').hidden = (presentes.length + semNome.length) > 0;

    // Sem a lista da turma, o aparelho gravou só o número do crachá: ninguém
    // sai dos faltantes por causa dele. Sem dizer isso, a aula parece ter mais
    // gente do que a turma inteira, e quem lê acha que a conta está errada.
    var nota = $('aula-nota');
    if (semNome.length) {
      nota.textContent = semNome.length === 1
        ? 'Um crachá foi lido sem a lista da turma: ele não tem nome e não sai dos faltantes.'
        : semNome.length + ' crachás foram lidos sem a lista da turma: eles não têm nome e não saem dos faltantes.';
      nota.hidden = false;
    } else {
      nota.hidden = true;
    }

    var faltantes = turma.filter(function (a) { return !vistos[a.matricula]; });
    $('conta-faltantes').textContent = '(' + faltantes.length + ')';
    faltantes.forEach(function (a) {
      var li = document.createElement('li');
      li.appendChild(texto('span', 'nome', a.nome));
      li.appendChild(texto('span', 'matricula', a.matricula));
      $('lista-faltantes').appendChild(li);
    });
    $('vazio-faltantes').hidden = faltantes.length > 0;
  }

  function linhaPresente(ev, nome) {
    var li = document.createElement('li');
    li.appendChild(texto('span', 'nome', nome || 'matrícula ' + ev.matricula));
    li.appendChild(texto('span', 'matricula', ev.matricula));
    if (ev.origem === 'manual') li.appendChild(texto('span', 'marca-manual', 'manual'));
    li.appendChild(texto('span', 'hora', horaCurta(ev.horario)));
    // O lançamento manual fica auditável também aqui: quem lançou e por quê.
    if (ev.origem === 'manual' && (ev.motivo || ev.lancado_por)) {
      var partes = [];
      if (ev.motivo) partes.push(ev.motivo);
      if (ev.lancado_por) partes.push('lançado por ' + ev.lancado_por);
      li.appendChild(texto('span', 'motivo', partes.join(' · ')));
    }
    return li;
  }

  function linhaSemNome(ev) {
    var li = document.createElement('li');
    li.appendChild(texto('span', 'nome', 'crachá ' + (ev.uid || '—')));
    li.appendChild(texto('span', 'matricula', 'sem matrícula'));
    li.appendChild(texto('span', 'hora', horaCurta(ev.horario)));
    return li;
  }

  /* ---------------------------------------------------------- partida --- */

  try { token = sessionStorage.getItem(GUARDA); } catch (e) { token = null; }
  if (token) carregarAulas(); else mostrar('login');

})();
