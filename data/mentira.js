'use strict';
/* Presente! — o aparelho de mentira.
 *
 * Responde as mesmas rotas que o `portal.cpp` vai responder e simula crachás
 * encostando no leitor, para as três telas rodarem no navegador sem depender
 * do ESP32 (issue #20). Os dados são inventados e moram neste arquivo.
 *
 * Quando o firmware existir, este arquivo e a linha que o carrega no
 * index.html são apagados: o `app.js` passa a falar por `fetch()` com o
 * aparelho de verdade e nada mais muda.
 *
 * Para experimentar, com a página aberta, pelo console do navegador:
 *   mentira.esperaMs = 14000     // uma espera longa, com o hotspot lento
 *   mentira.sincronizaFalha = true   // login sem internet: cai em modo offline
 *   mentira.envioFalha = true        // o Enviar falha e sobra o Compartilhar
 */
(function () {

  /* --------------------------------------------------------- ajustes ---- */
  var mentira = {
    esperaMs: 3000,          // quanto o "carregando turma…" demora
    sincronizaFalha: false,  // sem internet no login (R3, R14)
    envioFalha: false,       // o POST do relatório não sobe
    intervaloToqueMs: 2600,  // de quanto em quanto tempo um crachá encosta
    latenciaMs: 120          // o tanto que o aparelho leva para responder
  };

  /* ------------------------------------------------------ dados fixos ---- */
  var TURMA = '4172CMPN6A_P1';
  var DISCIPLINA = 'Sistemas Embarcados';

  // Contas locais do aparelho (Q4). No firmware a senha é hash SHA-256 + salt;
  // aqui é texto puro porque nada disto sai do navegador.
  var PROFESSORES = [
    { id: 'prof02', usuario: 'prof02', senha: 'uva2026', nome: 'Thiago Alberto Ramos Gabriel' }
  ];

  // A lista da turma que o aparelho baixa no login (R14). UID de 4 bytes é
  // Mifare; os de 7 bytes são NTAG215, e o firmware trata os dois igual (Q2).
  var ALUNOS = [
    { matricula: '1240110815', nome: 'Gabriel Albuquerque Varela Santarello', uid: 'A391F21B' },
    { matricula: '1240109764', nome: 'Cauã Manuel Proença de Andrade',        uid: '7C4E0D62' },
    { matricula: '1240114118', nome: 'Igor Rocha Lobato',                     uid: '04A2B7C91E5580' },
    { matricula: '1240205596', nome: 'Caio Parada Oliveira Planinschek',      uid: 'B20F5A74' },
    { matricula: '1240108001', nome: 'João Victor Berçot Chabudet Cabral',    uid: '3D81C4E0' },
    { matricula: '1240110222', nome: 'Ana Beatriz Nogueira',                  uid: '5F09A3D7' },
    { matricula: '1240111907', nome: 'Bruno Tavares Pinheiro',                uid: '04C1D8E32A6180' },
    { matricula: '1240112345', nome: 'Camila Rezende Monteiro',               uid: '9E62B015' },
    { matricula: '1240113088', nome: 'Diego Fonseca Arruda',                  uid: '1A7D4F83' },
    { matricula: '1240114765', nome: 'Elisa Martins do Vale',                 uid: 'C4308B29' },
    { matricula: '1240115430', nome: 'Felipe Andrade Quirino',                uid: '6B15E7A4' },
    { matricula: '1240116219', nome: 'Gustavo Lima Peçanha',                  uid: '04E97B221C3480' },
    { matricula: '1240117004', nome: 'Helena Duarte Bastos',                  uid: '2F8C60D1' },
    { matricula: '1240118552', nome: 'Isabela Coutinho Ramos',                uid: 'D053A9E6' },
    { matricula: '1240119370', nome: 'Jonas Ferreira Vasques',                uid: '78B241CF' },
    { matricula: '1240120145', nome: 'Karina Mendes Sobral',                  uid: 'E6147D3A' }
  ];

  // A ordem em que os crachás encostam. Repare em dois casos de propósito:
  // o crachá de fora da turma (vermelho, nada gravado) e o aluno que encosta
  // duas vezes — que na segunda dá JA_REGISTRADO e não gera evento (R30).
  var ROTEIRO = [
    'B20F5A74',        // Caio
    'A391F21B',        // Gabriel
    '9E62B015',        // Camila
    '7C4E0D62',        // Cauã
    'FFFFFFFF',        // crachá que não é da turma
    'B20F5A74',        // o Caio de novo, passados os 5 s da janela de silêncio
    'C4308B29',        // Elisa
    '04A2B7C91E5580',  // Igor (NTAG215)
    '2F8C60D1',        // Helena
    '5F09A3D7',        // Ana Beatriz
    '3D81C4E0',        // João Victor
    '78B241CF',        // Jonas
    '1A7D4F83',        // Diego
    '04C1D8E32A6180'   // Bruno (NTAG215)
  ];

  var JANELA_DE_SILENCIO_MS = 5000;   // Q5, redefinida pela R30

  /* ---------------------------------------------- estado do aparelho ---- */
  var st = zerar();

  function zerar() {
    return {
      autenticado: false,
      professor: null,
      sincronizando: false,
      modo: null,              // 'online' | 'offline', decidido no login (R14)
      roster: null,            // a lista da turma; null = aparelho sem lista
      sessao: null,            // { sessaoId, estado, inicio }
      eventos: [],
      registrados: {},         // uid -> true, para a regra de uma presença por sessão
      ultimaLeitura: {},       // uid -> instante, para a janela de silêncio
      ultimo: null,            // o que aconteceu no último toque
      passo: 0,                // onde o roteiro de toques está
      relogio: null            // id do temporizador dos toques
    };
  }

  /* --------------------------------------------------------- auxílio ---- */
  function pad(n) { return (n < 10 ? '0' : '') + n; }

  // ISO-8601 em -03:00, como manda o §7, independentemente do fuso da máquina.
  function iso(d) {
    var u = new Date(d.getTime() - 3 * 3600 * 1000);
    return u.getUTCFullYear() + '-' + pad(u.getUTCMonth() + 1) + '-' + pad(u.getUTCDate()) +
           'T' + pad(u.getUTCHours()) + ':' + pad(u.getUTCMinutes()) + ':' + pad(u.getUTCSeconds()) +
           '-03:00';
  }

  // O §7 fixa `<turma>-<AAAAMMDD>-<HHMM>`, com o relógio do Iniciar (R33).
  // No formato antigo, duas turmas no mesmo dia teriam o mesmo id, e o banco
  // recusaria o relatório da segunda como já enviado.
  function sessaoIdDe(d) {
    var u = new Date(d.getTime() - 3 * 3600 * 1000);
    return TURMA + '-' +
           u.getUTCFullYear() + pad(u.getUTCMonth() + 1) + pad(u.getUTCDate()) + '-' +
           pad(u.getUTCHours()) + pad(u.getUTCMinutes());
  }

  function alunoPorUid(uid) {
    if (!st.roster) return null;
    for (var i = 0; i < st.roster.length; i++) if (st.roster[i].uid === uid) return st.roster[i];
    return null;
  }

  function alunoPorMatricula(m) {
    if (!st.roster) return null;
    for (var i = 0; i < st.roster.length; i++) if (st.roster[i].matricula === m) return st.roster[i];
    return null;
  }

  function matriculaPresente(m) {
    for (var i = 0; i < st.eventos.length; i++) if (st.eventos[i].matricula === m) return true;
    return false;
  }

  function responder(valor) {
    return new Promise(function (ok) { setTimeout(function () { ok(valor); }, mentira.latenciaMs); });
  }

  /* ------------------------------------------------- leitura de tag ---- */
  // O caminho do §5: janela de silêncio, já registrado, fora da turma, grava.
  function encostou(uid) {
    var agora = Date.now();

    if (st.ultimaLeitura[uid] && agora - st.ultimaLeitura[uid] < JANELA_DE_SILENCIO_MS) return;
    st.ultimaLeitura[uid] = agora;

    if (st.registrados[uid]) {
      st.ultimo = { tipo: 'JA_REGISTRADO', nome: nomeDe(uid), timestamp: iso(new Date()) };
      return;                                    // verde, uma piscada, nada gravado
    }

    var aluno = alunoPorUid(uid);

    // Sem lista da turma o aparelho não sabe quem é da turma: aceita todo UID
    // e o enriquecimento acontece no envio (R3, HU-34).
    if (st.modo === 'online' && !aluno) {
      st.ultimo = { tipo: 'NAO_RECONHECIDO', uid: uid, timestamp: iso(new Date()) };
      return;                                    // vermelho, nada gravado
    }

    var evento = { uid: uid, timestamp: iso(new Date()), origem: 'nfc', enriquecido: !!aluno };
    if (aluno) { evento.matricula = aluno.matricula; evento.nome = aluno.nome; }

    st.eventos.push(evento);
    st.registrados[uid] = true;
    st.ultimo = { tipo: 'REGISTRADO', nome: aluno ? aluno.nome : null, uid: uid, timestamp: evento.timestamp };
  }

  function nomeDe(uid) {
    var a = alunoPorUid(uid);
    return a ? a.nome : null;
  }

  function ligarLeitor() {
    desligarLeitor();
    st.relogio = setInterval(function () {
      if (!st.sessao || st.sessao.estado !== 'aberta') return;
      if (st.passo >= ROTEIRO.length) return;
      encostou(ROTEIRO[st.passo++]);
    }, mentira.intervaloToqueMs);
  }

  function desligarLeitor() {
    if (st.relogio) { clearInterval(st.relogio); st.relogio = null; }
  }

  /* ------------------------------------------------------- relatório ---- */
  function montarRelatorio() {
    var comLista = !!st.roster;
    var rel = {
      sessao: {
        sessaoId: st.sessao.sessaoId,
        professorId: st.professor.id,
        disciplina: DISCIPLINA,
        turma: TURMA,
        inicio: st.sessao.inicio,
        modo: st.modo,
        relogio: 'portal'
      },
      tempos: ['t1', 't2'],
      roster_carregado: comLista,
      eventos: st.eventos.slice()
    };
    if (st.sessao.fim) rel.sessao.fim = st.sessao.fim;

    // presentes e faltantes só existem com a lista carregada (§7). Sem ela,
    // quem calcula é a API, no momento do envio.
    if (comLista) {
      var presentes = [], faltantes = [];
      for (var i = 0; i < st.roster.length; i++) {
        var a = st.roster[i];
        (matriculaPresente(a.matricula) ? presentes : faltantes).push(a.matricula);
      }
      rel.presentes = presentes;
      rel.faltantes = faltantes;
    }
    return rel;
  }

  // O recorte que as telas leem. O relatório do §7 é o de cima; este é a
  // mesma verdade arrumada para a tela, e é o que o portal vai buscar no
  // polling do painel.
  function montarEstado() {
    var e = {
      autenticado: st.autenticado,
      sincronizando: st.sincronizando,
      modo: st.modo,
      roster_carregado: !!st.roster,
      professor: st.professor ? { id: st.professor.id, nome: st.professor.nome } : null,
      turma: TURMA,
      disciplina: DISCIPLINA,
      total_alunos: st.roster ? st.roster.length : null,
      sessao: st.sessao ? { sessaoId: st.sessao.sessaoId, estado: st.sessao.estado, inicio: st.sessao.inicio } : null,
      ultimo: st.ultimo,
      presentes: [],
      faltantes: [],
      sem_nome: []
    };

    if (st.roster) {
      for (var i = 0; i < st.roster.length; i++) {
        var a = st.roster[i], achado = null;
        for (var j = 0; j < st.eventos.length; j++) {
          if (st.eventos[j].matricula === a.matricula) { achado = st.eventos[j]; break; }
        }
        if (achado) {
          e.presentes.push({
            matricula: a.matricula, nome: a.nome, timestamp: achado.timestamp,
            origem: achado.origem, lancado_por: achado.lancado_por || null, motivo: achado.motivo || null
          });
        } else {
          e.faltantes.push({ matricula: a.matricula, nome: a.nome });
        }
      }
      e.presentes.sort(function (x, y) { return x.timestamp < y.timestamp ? -1 : 1; });
    } else {
      for (var k = 0; k < st.eventos.length; k++) {
        e.sem_nome.push({
          uid: st.eventos[k].uid || null, matricula: st.eventos[k].matricula || null,
          timestamp: st.eventos[k].timestamp, origem: st.eventos[k].origem
        });
      }
    }
    return e;
  }

  /* ----------------------------------------------------------- rotas ---- */
  var rotas = {

    'GET /api/estado': function () { return montarEstado(); },

    'GET /api/relatorio': function () {
      if (!st.sessao) return { erro: 'SEM_SESSAO' };
      return montarRelatorio();
    },

    'POST /api/login': function (corpo) {
      var achado = null;
      for (var i = 0; i < PROFESSORES.length; i++) {
        if (PROFESSORES[i].usuario === corpo.usuario && PROFESSORES[i].senha === corpo.senha) {
          achado = PROFESSORES[i];
        }
      }
      if (!achado) return { erro: 'CREDENCIAL_INVALIDA' };

      st.autenticado = true;
      st.professor = achado;
      st.sincronizando = true;

      // A sincronização do R14: a resposta do login sai primeiro, e só então o
      // aparelho vai atrás da lista da turma. É essa espera que a tela mostra.
      setTimeout(function () {
        if (!st.autenticado) return;             // encerrou no meio da espera
        if (mentira.sincronizaFalha) { st.modo = 'offline'; st.roster = null; }
        else                         { st.modo = 'online';  st.roster = ALUNOS.slice(); }
        st.sincronizando = false;
      }, mentira.esperaMs);

      return { ok: true, sincronizando: true };
    },

    // Saída de emergência da espera: o professor desiste de esperar a lista e
    // manda a aula acontecer assim mesmo (R3, HU-31).
    'POST /api/seguir-offline': function () {
      if (!st.autenticado) return { erro: 'SEM_LOGIN' };
      st.modo = 'offline';
      st.roster = null;
      st.sincronizando = false;
      return { ok: true };
    },

    'POST /api/iniciar': function () {
      if (!st.autenticado) return { erro: 'SEM_LOGIN' };
      if (st.sessao) return { erro: 'SESSAO_JA_ABERTA' };
      var agora = new Date();
      st.sessao = { sessaoId: sessaoIdDe(agora), estado: 'aberta', inicio: iso(agora) };
      ligarLeitor();                              // o leitor passa a valer
      return { ok: true, sessao: st.sessao };
    },

    'POST /api/manual': function (corpo) {
      if (!st.sessao || st.sessao.estado !== 'aberta') return { erro: 'SEM_SESSAO' };

      var matricula = (corpo.matricula || '').trim();
      if (!matricula) return { erro: 'MATRICULA_VAZIA' };

      // Com a lista carregada o aparelho sabe quem é da turma e quem já está
      // presente, e recusa os dois casos (R30). Sem a lista ele não sabe
      // ligar matrícula a UID: grava, e a API junta no envio.
      var aluno = null;
      if (st.roster) {
        aluno = alunoPorMatricula(matricula);
        if (!aluno) return { erro: 'FORA_DA_TURMA' };
        if (matriculaPresente(matricula)) return { erro: 'JA_PRESENTE', nome: aluno.nome };
      }

      st.eventos.push({
        matricula: matricula,
        timestamp: iso(new Date()),
        origem: 'manual',
        lancado_por: st.professor.id,
        motivo: (corpo.motivo || '').trim() || 'não informado'
      });

      // Com a lista, o aparelho sabe o UID de quem foi lançado à mão, e o
      // crachá dessa pessoa passa a dar JA_REGISTRADO em vez de gravar um
      // segundo evento para a mesma matrícula (R30). Sem a lista ele não tem
      // como ligar matrícula a UID: grava os dois, e a API junta no envio.
      if (aluno) st.registrados[aluno.uid] = true;

      return { ok: true };
    },

    'POST /api/enviar': function () {
      if (!st.sessao) return { erro: 'SEM_SESSAO' };
      // A idempotência do R20: sessão finalizada recusa o segundo POST, e é
      // isso que impede o relatório duplicado quando o professor clica duas
      // vezes (HU-39).
      if (st.sessao.estado === 'finalizada') return { erro: 'JA_ENVIADO' };
      if (mentira.envioFalha) return { erro: 'SEM_INTERNET' };

      st.sessao.estado = 'finalizada';
      st.sessao.fim = iso(new Date());
      desligarLeitor();
      return { ok: true };
    },

    'POST /api/encerrar': function () {
      // LGPD por desenho (§6): a lista da turma e os eventos morrem aqui.
      desligarLeitor();
      st = zerar();
      return { ok: true };
    }
  };

  function despachar(metodo, rota, corpo) {
    var f = rotas[metodo + ' ' + rota];
    if (!f) return responder({ erro: 'ROTA_DESCONHECIDA' });
    return responder(f(corpo || {}));
  }

  /* -------------------------------------------------------- fachada ---- */
  window.aparelho = {
    get: function (rota) { return despachar('GET', rota); },
    post: function (rota, corpo) { return despachar('POST', rota, corpo); }
  };
  window.mentira = mentira;

  /* Recado na tela de login, para quem abrir o arquivo saber com o que
     entrar. Vive aqui de propósito: apagado o mentira.js, o recado some
     junto e o portal de verdade não nasce com dica de senha na tela. */
  document.addEventListener('DOMContentLoaded', function () {
    var alvo = document.getElementById('nota-de-mentira');
    if (!alvo) return;
    alvo.className = 'nota-mentira';
    alvo.innerHTML = 'Dados de mentira, sem aparelho nenhum atrás. ' +
      'Entre com <code>prof02</code> / <code>uva2026</code>.';
  });

})();
