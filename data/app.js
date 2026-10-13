'use strict';
/* Presente! — portal do professor.
 *
 * As quatro telas do §6 da spec e o que cada botão faz. Este arquivo não sabe
 * nada sobre os dados: tudo que ele mostra vem do aparelho, pelas funções
 * `pedir` e `enviar` logo abaixo. Elas falam por `fetch()` com o aparelho de
 * verdade, no contrato do §6.1 (R62); com `?mentira=1` no endereço, caem no
 * aparelho de mentira (mentira.js), que continua sendo o jeito de testar o
 * portal sem placa (R51).
 *
 * Ligar o portal ao aparelho não foi só trocar a ponte: quem diz se há sessão
 * é o cookie, quem escolhe a mensagem é o código de erro que vem no corpo, e o
 * Enviar espera mais que os outros pedidos porque ele vai à internet.
 */
(function () {

  /* ------------------------------------------------------- constantes ---- */

  // De quanto em quanto tempo a tela de espera pergunta se a lista já chegou.
  var ESPERA_INTERVALO_MS = 1000;

  // Quando a tela de espera desiste. Com o Plano A (R45) o rádio não troca, e a
  // espera é só a busca da lista pelo hotspot; os 25 s vieram da troca de rádio
  // do Plano B (5 a 15 s, R14 e R19), que caiu em 22/09, e ainda são curtos o
  // bastante para o professor não achar que o aparelho morreu. Valor de
  // partida, a calibrar na rodada do pacote 2 na placa (#44): o item 4 do
  // ticket `.scratch/presente/issues/09` foi fechado pela R62.
  var ESPERA_LIMITE_MS = 25000;

  // Quanto um pedido espera antes de a falha ser dada por certa (R40). O
  // aparelho está do outro lado da sala, na rede dele, e responde em
  // milissegundos; passado isso, ele não está respondendo. A busca da lista no
  // login é a exceção, e tem o limite acima. Valor de partida, a calibrar na
  // bancada junto dos outros tempos (§8).
  var PEDIDO_LIMITE_MS = 6000;

  // O Enviar é o único pedido que sai da sala: o aparelho tenta o banco por
  // até 20 s antes de responder `SEM_INTERNET` (§6.1). Esperar os 6 s dos
  // outros faria o portal desistir de um envio que ainda vai dar certo.
  var ENVIO_LIMITE_MS = 25000;

  // O painel relê o aparelho neste ritmo, para as contagens acompanharem os
  // crachás encostando. É consulta local, sem internet no meio (§6, passo 10).
  var PAINEL_INTERVALO_MS = 2000;

  /* ------------------------------------------------- ponte com o aparelho */

  // Com `?mentira=1` o portal fala com o aparelho de mentira; sem isso, com o
  // aparelho de verdade (R51). A decisão é lida uma vez, no carregamento, e
  // vale para o resto da vida da página.
  var DE_MENTIRA = /(^|[?&])mentira=1(&|$)/.test(window.location.search);

  // O pedido cru, antes do tempo-limite. Só aqui se sabe se do outro lado
  // está o `fetch` ou o aparelho de mentira.
  function bruto(metodo, rota, corpo, controle) {
    if (DE_MENTIRA) {
      return metodo === 'GET' ? window.aparelho.get(rota)
                              : window.aparelho.post(rota, corpo || {});
    }
    var opcoes = {
      method: metodo,
      headers: { 'Content-Type': 'application/json' },
      // O cookie `sessao` é quem diz que há login (§6.1); sem mandá-lo, toda
      // rota responderia 401.
      credentials: 'same-origin',
      cache: 'no-store'
    };
    // Pedido sem dados manda `{}` (§6.1). O GET não manda corpo nenhum.
    if (metodo !== 'GET') opcoes.body = JSON.stringify(corpo || {});
    if (controle) opcoes.signal = controle.signal;
    return fetch(rota, opcoes).then(lerResposta);
  }

  // JSON nos dois sentidos, e é o código de `erro` do corpo que escolhe a
  // mensagem, não o número do status (§6.1). Resposta de erro com corpo
  // ilegível não pode virar sucesso silencioso: vira um código sintético, que
  // cai na mensagem genérica de quem chamou.
  function lerResposta(resp) {
    return resp.text().then(function (texto_) {
      var corpo = null;
      if (texto_) { try { corpo = JSON.parse(texto_); } catch (e) { corpo = null; } }
      if (resp.ok) return corpo || {};
      if (corpo && corpo.erro) return corpo;
      return { erro: 'HTTP_' + resp.status };
    });
  }

  // Nenhuma tela avança sem o aparelho confirmar (R40). O tempo-limite mora
  // aqui, na única porta por onde todos os pedidos passam: estourado, a falha
  // aparece na tela, os botões voltam a aceitar toque, e a promessa não
  // continua. É o `.then()` que não roda que impede a tela de trocar com o
  // aparelho mudo — o caso em que trocar de tela seria mais enganoso do que
  // não fazer nada.
  function comAparelho(metodo, rota, corpo, limiteMs) {
    return new Promise(function (ok) {
      var encerrado = false;
      var controle = (!DE_MENTIRA && typeof AbortController === 'function')
        ? new AbortController() : null;

      var relogio = setTimeout(function () {
        encerrado = true;
        // O pedido estourado também morre do lado da rede. Sem isto ele fica
        // pendurado, e o aparelho atende um de cada vez: o próximo toque
        // entraria na fila atrás de uma resposta que ninguém mais vai ler.
        if (controle) controle.abort();
        falhouOAparelho();
      }, limiteMs);

      bruto(metodo, rota, corpo, controle).then(function (r) {
        clearTimeout(relogio);
        if (encerrado) return;
        aparelhoRespondeu();
        // 401 `SEM_LOGIN` em qualquer rota leva o portal ao login (§6.1).
        // Quem ficasse na tela estaria lendo uma sessão que não existe mais.
        if (r && r.erro === 'SEM_LOGIN') { perdeuASessao(); return; }
        ok(r);
      }, function () {
        clearTimeout(relogio);
        if (encerrado) return;
        encerrado = true;
        falhouOAparelho();
      });
    });
  }

  function pedir(rota) { return comAparelho('GET', rota, null, PEDIDO_LIMITE_MS); }

  function enviar(rota, corpo) {
    // O Enviar é o único que espera 25 s: ele vai à internet (§6.1).
    var limite = rota === '/api/enviar' ? ENVIO_LIMITE_MS : PEDIDO_LIMITE_MS;
    return comAparelho('POST', rota, corpo || {}, limite);
  }

  function perdeuASessao() {
    if (telaAtual === 'login') return;
    irParaLogin();
  }

  // A hora do celular, que o aparelho não tem: ele não leva relógio de
  // bateria, e é o `hora` do login e do Iniciar que ancora os horários da
  // chamada (§6.1). ISO-8601 em -03:00, como o resto do contrato.
  function horaDoCelular() {
    var d = new Date();
    function p(n) { return (n < 10 ? '0' : '') + n; }
    return d.getFullYear() + '-' + p(d.getMonth() + 1) + '-' + p(d.getDate()) +
           'T' + p(d.getHours()) + ':' + p(d.getMinutes()) + ':' + p(d.getSeconds()) +
           fusoDoCelular(d);
  }

  // O aparelho espera -03:00 (§6.1), mas quem abre o portal pode estar com o
  // celular noutro fuso; mandar -03:00 fixo gravaria a hora errada.
  function fusoDoCelular(d) {
    var min = -d.getTimezoneOffset();
    var sinal = min < 0 ? '-' : '+';
    min = Math.abs(min);
    function p(n) { return (n < 10 ? '0' : '') + n; }
    return sinal + p(Math.floor(min / 60)) + ':' + p(min % 60);
  }

  /* ------------------------------------------------------------ atalhos -- */

  function $(id) { return document.getElementById(id); }

  function texto(tag, classe, conteudo) {
    var el = document.createElement(tag);
    if (classe) el.className = classe;
    if (conteudo !== undefined && conteudo !== null) el.textContent = conteudo;
    return el;
  }

  function hora(ts) { return ts ? ts.slice(11, 16) : ''; }

  /* ------------------------------------------------------------ estado --- */

  var telaAtual = null;
  var relogioEspera = null;
  var relogioPainel = null;
  var esperaComecouEm = 0;
  var enviandoAgora = false;
  var aparelhoMudo = false;      // o último pedido estourou o tempo-limite (R40)

  // As listas de presentes, faltantes e crachás sem nome, guardadas da última
  // vez que o /api/presencas foi pedido, e a `versao` que elas tinham. O
  // estado traz a `versao` corrente; enquanto ela não mudar, estas servem.
  var listas = null;
  var versaoDasListas = null;

  var TELAS = ['login', 'espera', 'painel', 'relatorio'];

  function mostrar(nome) {
    TELAS.forEach(function (t) { $('tela-' + t).hidden = (t !== nome); });
    telaAtual = nome;

    pararEspera();
    pararPainel();
    if (nome === 'espera') comecarEspera();
    if (nome === 'painel' || nome === 'relatorio') comecarPainel();

    // Leitor de tela e teclado precisam saber que a tela trocou; sem isto o
    // foco fica preso no botão da tela anterior, que agora está escondida.
    var titulo = $('tela-' + nome).querySelector('h1');
    if (titulo) { titulo.tabIndex = -1; titulo.focus(); }
    window.scrollTo(0, 0);
  }

  function recado(texto_, tipo) {
    var el = $('painel-recado');
    if (!texto_) { el.hidden = true; el.textContent = ''; return; }
    el.textContent = texto_;
    el.className = 'aviso-vivo' + (tipo === 'bom' ? ' bom' : '');
    el.hidden = false;
  }

  /* --------------------------------------------- aparelho que não responde */

  function falhouOAparelho() {
    enviandoAgora = false;

    // O que o pedido travou volta a aceitar toque: sem isto a nova tentativa
    // que a mensagem pede não seria possível.
    ['botao-entrar', 'acao-iniciar', 'acao-enviar'].forEach(function (id) {
      $(id).disabled = false;
    });

    // Na espera do login quem avisa é a própria tela, com o contador e o
    // limite dela; escrever aqui encheria a mesma faixa duas vezes. Depois do
    // estouro, porém, os dois botões de lá são pedidos como os outros.
    if (telaAtual === 'espera' && $('espera-estourou').hidden) return;

    aparelhoMudo = true;

    // Dizer que esperar resolve é o que separa "tente de novo" de "desista".
    // O aparelho não some: ele para de atender enquanto busca a turma ou
    // envia, e volta sozinho — a busca desiste em 20 s (R62) e a trava da
    // conexão segura passa por conta própria (R68).
    var aviso = 'O aparelho não respondeu. Ele se recupera sozinho em alguns ' +
                'segundos — espere e toque de novo.';

    if (telaAtual === 'login') {
      $('erro-login').textContent = aviso;
      $('erro-login').hidden = false;
    } else if (telaAtual === 'espera') {
      recadoDaEspera(aviso);
    } else if (telaAtual === 'relatorio') {
      $('relatorio-recado').textContent = aviso;
      $('relatorio-recado').hidden = false;
    } else {
      recado(aviso);
    }

    // O modal fica por cima de tudo, e o que está atrás dele não se lê.
    var err = $('erro-manual');
    if (err && !$('fundo-modal').hidden) { err.textContent = aviso; err.hidden = false; }
  }

  function aparelhoRespondeu() {
    if (!aparelhoMudo) return;
    aparelhoMudo = false;
    if (telaAtual === 'login') $('erro-login').hidden = true;
    else if (telaAtual === 'espera') recadoDaEspera('');
    else if (telaAtual === 'relatorio') $('relatorio-recado').hidden = true;
    else recado('');
  }

  /* -------------------------------------------------------------- modal -- */

  var modalAoConfirmar = null;
  var focoAntesDoModal = null;

  function abrirModal(titulo, montarCorpo, rotuloConfirmar, aoConfirmar) {
    focoAntesDoModal = document.activeElement;
    $('modal-titulo').textContent = titulo;
    var corpo = $('modal-corpo');
    corpo.innerHTML = '';
    if (montarCorpo) montarCorpo(corpo);
    $('modal-confirmar').textContent = rotuloConfirmar;
    modalAoConfirmar = aoConfirmar;
    $('fundo-modal').hidden = false;
    var primeiro = corpo.querySelector('input') || $('modal-confirmar');
    if (primeiro) primeiro.focus();
  }

  function fecharModal() {
    $('fundo-modal').hidden = true;
    modalAoConfirmar = null;
    // Devolve o foco a quem abriu a caixa; sem isto ele volta para o começo da
    // página e o professor perde o lugar.
    if (focoAntesDoModal && focoAntesDoModal.focus) focoAntesDoModal.focus();
    focoAntesDoModal = null;
  }

  $('modal-cancelar').addEventListener('click', fecharModal);
  $('modal-confirmar').addEventListener('click', function () {
    if (modalAoConfirmar) modalAoConfirmar();
  });
  $('fundo-modal').addEventListener('click', function (ev) {
    if (ev.target === $('fundo-modal')) fecharModal();
  });
  document.addEventListener('keydown', function (ev) {
    if ($('fundo-modal').hidden) return;
    if (ev.key === 'Escape') { fecharModal(); return; }
    if (ev.key !== 'Tab') return;
    // A caixa declara aria-modal: o foco não pode passear pelos botões do
    // painel, que continuam atrás dela.
    var focaveis = $('fundo-modal').querySelectorAll('input, button');
    if (!focaveis.length) return;
    var primeiro = focaveis[0], ultimo = focaveis[focaveis.length - 1];
    if (ev.shiftKey && document.activeElement === primeiro) { ev.preventDefault(); ultimo.focus(); }
    else if (!ev.shiftKey && document.activeElement === ultimo) { ev.preventDefault(); primeiro.focus(); }
  });

  /* ------------------------------------------------------- 1. LOGIN ----- */

  $('form-login').addEventListener('submit', function (ev) {
    // Trava o envio nativo do formulário. Sem isto o navegador troca de
    // página e, num GET, escreveria usuário e senha na barra de endereço —
    // o que a R25 proíbe.
    ev.preventDefault();

    var usuario = $('usuario').value.trim();
    var senha = $('senha').value;
    var erro = $('erro-login');

    if (!usuario || !senha) {
      erro.textContent = 'Preencha usuário e senha.';
      erro.hidden = false;
      return;
    }

    erro.hidden = true;
    $('botao-entrar').disabled = true;

    // A hora vai junto do login e do Iniciar (§6.1): se ela faltar aqui, o
    // aparelho não recusa o login — espera a do Iniciar.
    enviar('/api/login', { usuario: usuario, senha: senha, hora: horaDoCelular() }).then(function (r) {
      $('botao-entrar').disabled = false;
      if (r.erro) {
        erro.textContent = r.erro === 'CREDENCIAL_INVALIDA'
          ? 'Usuário ou senha não conferem.'
          : 'O aparelho não aceitou o login.';
        erro.hidden = false;
        $('senha').value = '';
        $('senha').focus();
        return;
      }
      // A senha sai da memória da página assim que deixa de ser necessária.
      $('senha').value = '';
      // A espera pergunta o estado assim que abre: com o aparelho em
      // AGUARDANDO_LOGIN ela mostra a busca da turma, e, retomando uma chamada
      // em curso (R49), não há busca nenhuma e ela passa direto ao painel.
      mostrar('espera');
    });
  });

  /* ------------------------------------------------------ 2. ESPERA ----- */

  function comecarEspera() {
    esperaComecouEm = Date.now();
    $('espera-estourou').hidden = true;
    recadoDaEspera('');
    $('tela-espera').querySelector('.giro').hidden = false;
    $('espera-segundos').textContent = '0';
    relogioEspera = setInterval(baterEspera, ESPERA_INTERVALO_MS);
    baterEspera();
  }

  function pararEspera() {
    if (relogioEspera) { clearInterval(relogioEspera); relogioEspera = null; }
  }

  function recadoDaEspera(texto_) {
    var el = $('espera-recado');
    el.textContent = texto_ || '';
    el.hidden = !texto_;            // sem texto a faixa some, em vez de virar caixa vazia
  }

  function baterEspera() {
    var passou = Date.now() - esperaComecouEm;
    $('espera-segundos').textContent = String(Math.floor(passou / 1000));

    // Dizer o que está acontecendo é o que separa "esperando" de "travado".
    if (passou > 6000) {
      recadoDaEspera('Ainda buscando. Se o celular avisar que saiu da rede do aparelho, ele volta sozinho.');
    }

    if (passou > ESPERA_LIMITE_MS) {
      pararEspera();
      $('tela-espera').querySelector('.giro').hidden = true;
      recadoDaEspera('');
      $('espera-estourou').hidden = false;
      return;
    }

    pedir('/api/estado').then(function (e) {
      if (telaAtual !== 'espera') return;
      if (!e.autenticado) { mostrar('login'); return; }
      if (!e.sincronizando) mostrar('painel');
    });
  }

  $('botao-seguir-offline').addEventListener('click', function () {
    enviar('/api/seguir-offline', {}).then(function () { mostrar('painel'); });
  });

  $('botao-voltar-login').addEventListener('click', function () {
    enviar('/api/encerrar', {}).then(function () { irParaLogin(); });
  });

  /* ------------------------------------------------------ 3. PAINEL ----- */

  function comecarPainel() {
    relogioPainel = setInterval(atualizarPainel, PAINEL_INTERVALO_MS);
    atualizarPainel();
  }

  function pararPainel() {
    if (relogioPainel) { clearInterval(relogioPainel); relogioPainel = null; }
  }

  function atualizarPainel() {
    pedir('/api/estado').then(function (e) {
      if (!e.autenticado) { irParaLogin(); return; }
      pintarTopo(e);
      if (telaAtual === 'painel') { pintarPainel(e); return; }
      if (telaAtual !== 'relatorio') return;

      // As listas vêm do /api/presencas, e só são pedidas quando a `versao` do
      // estado muda (§6.1). O painel bate de 2 em 2 s; pedir as listas a cada
      // batida faria o aparelho remontar, a cada duas segundos, o que não
      // mudou — e ele atende um pedido de cada vez.
      if (listas && e.versao === versaoDasListas) { pintarRelatorio(e); return; }

      pedir('/api/presencas').then(function (p) {
        if (telaAtual !== 'relatorio') return;
        if (!p || p.erro) return;
        listas = p;
        versaoDasListas = (p.versao === undefined) ? e.versao : p.versao;
        pintarRelatorio(e);
      });
    });
  }

  function pintarTopo(e) {
    var t = $('topo-estado');
    if (!e.autenticado) { t.hidden = true; return; }
    t.textContent = e.turma;
    t.hidden = false;
  }

  function pintarPainel(e) {
    $('painel-quem').textContent = e.professor
      ? e.professor.nome + ' · ' + e.disciplina
      : e.disciplina;

    var aberta = !!e.sessao && e.sessao.estado === 'aberta';
    var finalizada = !!e.sessao && e.sessao.estado === 'finalizada';

    var pSessao = $('pilula-sessao');
    // Enviar fecha a chamada (R46): depois dele o leitor para de registrar, e
    // a pílula tem que dizer isso, e não só que o relatório subiu.
    pSessao.textContent = aberta ? 'Chamada aberta'
                        : (finalizada ? 'Chamada fechada' : 'Chamada não começou');
    pSessao.className = 'pilula' + (aberta ? ' viva' : '');

    var pModo = $('pilula-modo');
    if (e.modo === 'offline') {
      pModo.textContent = 'Sem a lista da turma';
      pModo.className = 'pilula atencao';
      pModo.hidden = false;
    } else if (e.modo === 'online') {
      pModo.textContent = 'Lista carregada';
      pModo.className = 'pilula';
      pModo.hidden = false;
    } else {
      pModo.hidden = true;
    }

    // Sem a lista da turma não há total nem faltantes: só o que foi lido (§7).
    // A contagem sai dos números do próprio estado, e não do tamanho das
    // listas: desde a R62 elas só vêm no `/api/presencas`, e ler `.length` de
    // uma lista que não chega parava o painel aqui, sem contagem, sem último
    // toque e sem atualizar os botões.
    if (e.roster_carregado) {
      $('painel-contagem').textContent = (e.n_presentes || 0) + ' de ' + e.total_alunos + ' presentes';
    } else {
      $('painel-contagem').textContent = (e.n_eventos || 0) + ' crachás lidos';
    }

    $('painel-ultimo').textContent = descreverUltimo(e);

    $('acao-iniciar').disabled = !!e.sessao;
    $('acao-manual').disabled = !aberta;
    $('acao-exibir').disabled = !e.sessao;
    $('acao-compartilhar').disabled = !e.sessao;
    $('acao-enviar').disabled = !aberta || enviandoAgora;
  }

  function descreverUltimo(e) {
    var u = e.ultimo;
    if (!u) return '';
    var quem = u.nome || (u.uid ? 'crachá ' + u.uid : 'crachá');
    if (u.tipo === 'REGISTRADO') return hora(u.timestamp) + ' · ' + quem + ' registrado';
    if (u.tipo === 'JA_REGISTRADO') return hora(u.timestamp) + ' · ' + quem + ' já estava registrado';
    if (u.tipo === 'NAO_RECONHECIDO') return hora(u.timestamp) + ' · ' + quem + ' não é da turma';
    return '';
  }

  /* --- Iniciar ----------------------------------------------------------- */
  $('acao-iniciar').addEventListener('click', function () {
    recado('');
    // O toque duplo é comum no celular, e quem desabilita o botão pelo estado
    // é o polling, que só chega depois da resposta. Sem esta trava o segundo
    // toque recebe SESSAO_JA_ABERTA e a tela diria que a chamada não abriu
    // justamente com ela aberta.
    $('acao-iniciar').disabled = true;
    // O Iniciar manda a hora do celular: é ela que data a chamada (§6.1).
    enviar('/api/iniciar', { hora: horaDoCelular() }).then(function (r) {
      if (r.erro && r.erro !== 'SESSAO_JA_ABERTA') {
        recado(r.erro === 'HORA_INVALIDA'
          ? 'O aparelho não aceitou a hora do celular. Confira a data e a hora dele e tente de novo.'
          : 'Não foi possível abrir a chamada.');
        atualizarPainel();
        return;
      }
      recado('Chamada aberta. Pode encostar os crachás.', 'bom');
      atualizarPainel();
    });
  });

  /* --- Adicionar presença manual ---------------------------------------- */
  $('acao-manual').addEventListener('click', function () {
    recado('');
    abrirModal('Presença manual', function (corpo) {
      corpo.appendChild(texto('p', 'sub', 'Para o aluno que veio sem o crachá.'));

      var lm = texto('label', null, 'Matrícula');
      lm.setAttribute('for', 'campo-matricula');
      var im = document.createElement('input');
      im.id = 'campo-matricula';
      im.type = 'text';
      im.inputMode = 'numeric';
      im.autocomplete = 'off';

      var lr = texto('label', null, 'Motivo');
      lr.setAttribute('for', 'campo-motivo');
      var ir = document.createElement('input');
      ir.id = 'campo-motivo';
      ir.type = 'text';
      ir.autocomplete = 'off';
      ir.placeholder = 'esqueceu a tag';

      var err = texto('p', 'erro', '');
      err.id = 'erro-manual';
      err.hidden = true;

      corpo.appendChild(lm); corpo.appendChild(im);
      corpo.appendChild(lr); corpo.appendChild(ir);
      corpo.appendChild(err);
    }, 'Lançar', function () {
      var matricula = $('campo-matricula').value.trim();
      var motivo = $('campo-motivo').value.trim();
      var err = $('erro-manual');

      enviar('/api/manual', { matricula: matricula, motivo: motivo }).then(function (r) {
        if (r.erro) {
          // O aviso fica na própria tela, e não num alerta que some: o
          // professor precisa ler por que o lançamento não passou (R30).
          err.textContent = mensagemDoManual(r);
          err.hidden = false;
          return;
        }
        fecharModal();
        recado('Presença lançada para a matrícula ' + matricula + '.', 'bom');
        atualizarPainel();
      });
    });
  });

  function mensagemDoManual(r) {
    if (r.erro === 'JA_PRESENTE') {
      return (r.nome ? r.nome : 'Esta matrícula') + ' já está presente nesta chamada. Nada foi lançado.';
    }
    if (r.erro === 'FORA_DA_TURMA') return 'Esta matrícula não está na lista da turma.';
    if (r.erro === 'MATRICULA_VAZIA') return 'Informe a matrícula.';
    if (r.erro === 'SEM_SESSAO') return 'A chamada não está aberta.';
    return 'O aparelho recusou o lançamento.';
  }

  /* --- Exibir relatório -------------------------------------------------- */
  $('acao-exibir').addEventListener('click', function () {
    recado('');
    mostrar('relatorio');
  });

  $('acao-voltar').addEventListener('click', function () { mostrar('painel'); });

  /* --- Compartilhar relatório -------------------------------------------- */
  $('acao-compartilhar').addEventListener('click', function () {
    recado('');
    pedir('/api/relatorio').then(function (rel) {
      if (rel.erro) { recado('Não há relatório para compartilhar ainda.'); return; }
      baixar(rel);
    });
  });

  function baixar(rel) {
    var conteudo = JSON.stringify(rel, null, 2);
    var nome = 'presente-' + rel.sessao.sessaoId + '.json';
    try {
      var url = URL.createObjectURL(new Blob([conteudo], { type: 'application/json' }));
      var a = document.createElement('a');
      a.href = url;
      a.download = nome;
      document.body.appendChild(a);
      a.click();
      document.body.removeChild(a);
      setTimeout(function () { URL.revokeObjectURL(url); }, 2000);
      recado('Relatório salvo como ' + nome + '.', 'bom');
    } catch (e) {
      // Safari no iPhone ignora o download e abre o arquivo numa aba. Se nem
      // isso funcionar, o relatório vai para a tela, de onde dá para copiar.
      abrirModal('Relatório', function (corpo) {
        var pre = texto('pre', null, conteudo);
        pre.style.whiteSpace = 'pre-wrap';
        pre.style.wordBreak = 'break-all';
        pre.style.fontSize = '.75rem';
        corpo.appendChild(pre);
      }, 'Fechar', fecharModal);
    }
  }

  /* --- Enviar relatório -------------------------------------------------- */
  $('acao-enviar').addEventListener('click', function () {
    recado('');
    if (enviandoAgora) return;

    // Duas travas contra o envio duplicado (HU-39): esta, que fecha a janela
    // entre o clique e a resposta, e a do aparelho, que com a chamada já
    // fechada responde `ja_enviado` sem ir à internet (R62) — ele não recusa o
    // segundo POST, e por isso não há mais ramo de erro para esse caso.
    enviandoAgora = true;
    $('acao-enviar').disabled = true;
    recado('Enviando e fechando a chamada…');

    enviar('/api/enviar', {}).then(function (r) {
      enviandoAgora = false;
      if (r.erro) {
        recado(mensagemDoEnvio(r.erro));
      } else if (r.resultado === 'ja_enviado') {
        recado('Esta chamada já tinha sido enviada. Nada foi duplicado.');
      } else {
        recado('Relatório enviado. A chamada está fechada.', 'bom');
      }
      atualizarPainel();
    });
  });

  // As recusas do Enviar (§6.1). O que muda entre elas é o que o professor
  // faz a seguir: sem internet, tentar de novo adianta; com o banco recusando,
  // não — o jeito é salvar pelo Compartilhar e resolver depois.
  function mensagemDoEnvio(erro) {
    if (erro === 'SEM_SESSAO') return 'Nenhuma chamada foi iniciada ainda.';
    if (erro === 'SEM_INTERNET') {
      return 'O aparelho não chegou à internet. A chamada continua aberta — ' +
             'tente de novo, ou salve com Compartilhar.';
    }
    if (erro === 'BANCO_RECUSOU') {
      return 'O banco recusou o relatório. A chamada continua aberta — ' +
             'salve com Compartilhar e avise o grupo.';
    }
    return 'O envio falhou. O relatório continua no aparelho — use Compartilhar para salvar no celular.';
  }

  /* --- Encerrar ---------------------------------------------------------- */
  $('acao-encerrar').addEventListener('click', function () {
    recado('');
    abrirModal('Encerrar a chamada?', function (corpo) {
      corpo.appendChild(texto('p', null,
        'O aparelho apaga a lista da turma e as presenças desta chamada e volta ao login.'));
      corpo.appendChild(texto('p', 'sub',
        'Se o relatório ainda não foi enviado, salve com Compartilhar antes.'));
    }, 'Encerrar', function () {
      enviar('/api/encerrar', {}).then(function () {
        fecharModal();
        irParaLogin();
      });
    });
  });

  function irParaLogin() {
    $('usuario').value = '';
    $('senha').value = '';
    $('erro-login').hidden = true;
    $('topo-estado').hidden = true;
    // Voltando ao login, o portal esquece as listas guardadas (§6.1): elas são
    // de uma chamada que acabou, e o próximo professor não pode vê-las.
    listas = null;
    versaoDasListas = null;
    recado('');
    mostrar('login');
  }

  /* --------------------------------------------------- 4. RELATÓRIO ----- */

  function pintarRelatorio(e) {
    var cab = [];
    if (e.professor) cab.push(e.professor.nome);
    cab.push(e.disciplina);
    cab.push(e.turma);
    if (e.sessao) cab.push('início ' + hora(e.sessao.inicio));
    $('relatorio-cabecalho').textContent = cab.join(' · ');

    $('relatorio-offline').hidden = e.roster_carregado;
    $('titulo-faltantes').hidden = !e.roster_carregado;

    // Sem as listas ainda na mão não há o que desenhar: o cabeçalho já saiu, e
    // o resto entra quando o /api/presencas responder.
    if (!listas) return;

    var presentes = $('lista-presentes');
    presentes.innerHTML = '';

    if (e.roster_carregado) {
      var osPresentes = listas.presentes || [];
      $('conta-presentes').textContent = '(' + osPresentes.length + ')';
      osPresentes.forEach(function (p) { presentes.appendChild(linhaPresente(p)); });
      $('vazio-presentes').hidden = osPresentes.length > 0;

      var faltantes = $('lista-faltantes');
      faltantes.innerHTML = '';
      var osFaltantes = listas.faltantes || [];
      $('conta-faltantes').textContent = '(' + osFaltantes.length + ')';
      osFaltantes.forEach(function (f) {
        var li = document.createElement('li');
        li.appendChild(texto('span', 'nome', f.nome));
        li.appendChild(texto('span', 'matricula', f.matricula));
        faltantes.appendChild(li);
      });
      $('vazio-faltantes').hidden = osFaltantes.length > 0;
    } else {
      // Sem a lista da turma o relatório é a fila de crachás lidos, sem nome
      // e sem faltantes — os nomes entram no envio (R3, HU-32).
      var semNome = listas.sem_nome || [];
      $('conta-presentes').textContent = '(' + semNome.length + ')';
      semNome.forEach(function (s) {
        var li = document.createElement('li');
        li.appendChild(texto('span', 'nome', s.matricula ? 'matrícula ' + s.matricula : 'crachá ' + s.uid));
        if (s.origem === 'manual') li.appendChild(texto('span', 'marca-manual', 'manual'));
        li.appendChild(texto('span', 'hora', hora(s.timestamp)));
        presentes.appendChild(li);
      });
      $('vazio-presentes').hidden = semNome.length > 0;
      $('lista-faltantes').innerHTML = '';
      $('conta-faltantes').textContent = '';
      $('vazio-faltantes').hidden = true;
    }
  }

  function linhaPresente(p) {
    var li = document.createElement('li');
    li.appendChild(texto('span', 'nome', p.nome));
    li.appendChild(texto('span', 'matricula', p.matricula));
    if (p.origem === 'manual') li.appendChild(texto('span', 'marca-manual', 'manual'));
    li.appendChild(texto('span', 'hora', hora(p.timestamp)));
    // O lançamento manual fica auditável na própria tela: quem lançou e por
    // que (HU-10).
    if (p.origem === 'manual' && (p.motivo || p.lancado_por)) {
      var partes = [];
      if (p.motivo) partes.push(p.motivo);
      if (p.lancado_por) partes.push('lançado por ' + p.lancado_por);
      li.appendChild(texto('span', 'motivo', partes.join(' · ')));
    }
    return li;
  }

  /* ---------------------------------------------------------- partida --- */

  // Ao abrir, pergunta o estado antes de deixar a tela de login: se a chamada
  // já está aberta e o cookie ainda vale, recarregar a página volta ao painel
  // em vez de pedir a senha de novo (R49). A tela de login aparece primeiro
  // porque o estado pode demorar — e, se o aparelho não responder, é nela que
  // a falha tem de ser lida.
  function comecar() {
    mostrar('login');
    pedir('/api/estado').then(function (e) {
      if (!e || !e.autenticado) return;
      mostrar(e.sincronizando ? 'espera' : 'painel');
    });
  }

  // O mentira.js só entra quando o endereço pede (R51). Carregá-lo aqui, e não
  // por uma tag fixa no HTML, é o que garante que o portal de verdade não leve
  // junto nem o aparelho de mentira nem a dica de senha que ele escreve na
  // tela de login.
  if (DE_MENTIRA) {
    var tag = document.createElement('script');
    tag.src = 'mentira.js';
    tag.onload = comecar;
    tag.onerror = function () {
      mostrar('login');
      $('erro-login').textContent = 'O aparelho de mentira não carregou. ' +
        'Tire o ?mentira=1 do endereço para falar com o aparelho de verdade.';
      $('erro-login').hidden = false;
    };
    document.head.appendChild(tag);
  } else {
    comecar();
  }

})();
