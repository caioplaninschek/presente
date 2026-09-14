'use strict';
/* Presente! — portal do professor.
 *
 * As quatro telas do §6 da spec e o que cada botão faz. Este arquivo não sabe
 * nada sobre os dados: tudo que ele mostra vem do aparelho, pelas funções
 * `pedir` e `enviar` logo abaixo. Hoje elas caem no aparelho de mentira
 * (mentira.js); com o firmware pronto, viram `fetch()` e o resto fica igual.
 */
(function () {

  /* ------------------------------------------------------- constantes ---- */

  // De quanto em quanto tempo a tela de espera pergunta se a lista já chegou.
  var ESPERA_INTERVALO_MS = 1000;

  // Quando a tela de espera desiste. A troca de rádio do Plano B leva de 5 a
  // 15 s (R13); 25 s dá margem e ainda é curto o bastante para o professor não
  // achar que o aparelho morreu. Valor de partida, a calibrar na bancada — o
  // item 4 do ticket `.scratch/presente/issues/09` ainda está em aberto.
  var ESPERA_LIMITE_MS = 25000;

  // O painel relê o aparelho neste ritmo, para as contagens acompanharem os
  // crachás encostando. É consulta local, sem internet no meio (§6, passo 10).
  var PAINEL_INTERVALO_MS = 2000;

  /* ------------------------------------------------- ponte com o aparelho */

  function pedir(rota) { return window.aparelho.get(rota); }
  function enviar(rota, corpo) { return window.aparelho.post(rota, corpo); }

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

    enviar('/api/login', { usuario: usuario, senha: senha }).then(function (r) {
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
      if (telaAtual === 'painel') pintarPainel(e);
      if (telaAtual === 'relatorio') pintarRelatorio(e);
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
    pSessao.textContent = aberta ? 'Chamada aberta' : (finalizada ? 'Relatório enviado' : 'Chamada fechada');
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
    if (e.roster_carregado) {
      $('painel-contagem').textContent = e.presentes.length + ' de ' + e.total_alunos + ' presentes';
    } else {
      $('painel-contagem').textContent = e.sem_nome.length + ' crachás lidos';
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
    enviar('/api/iniciar', {}).then(function (r) {
      if (r.erro && r.erro !== 'SESSAO_JA_ABERTA') {
        recado('Não foi possível abrir a chamada.');
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
    // entre o clique e a resposta, e a do aparelho, que recusa o segundo POST
    // porque a sessão já está finalizada (R20). A daqui é conveniência; a de
    // lá é a que vale.
    enviandoAgora = true;
    $('acao-enviar').disabled = true;
    recado('Enviando o relatório…');

    enviar('/api/enviar', {}).then(function (r) {
      enviandoAgora = false;
      if (r.erro === 'JA_ENVIADO') {
        recado('Este relatório já foi enviado. Nada foi duplicado.');
      } else if (r.erro) {
        recado('O envio falhou. O relatório continua no aparelho — use Compartilhar para salvar no celular.');
      } else {
        recado('Relatório enviado.', 'bom');
      }
      atualizarPainel();
    });
  });

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

    var presentes = $('lista-presentes');
    presentes.innerHTML = '';

    if (e.roster_carregado) {
      $('conta-presentes').textContent = '(' + e.presentes.length + ')';
      e.presentes.forEach(function (p) { presentes.appendChild(linhaPresente(p)); });
      $('vazio-presentes').hidden = e.presentes.length > 0;

      var faltantes = $('lista-faltantes');
      faltantes.innerHTML = '';
      $('conta-faltantes').textContent = '(' + e.faltantes.length + ')';
      e.faltantes.forEach(function (f) {
        var li = document.createElement('li');
        li.appendChild(texto('span', 'nome', f.nome));
        li.appendChild(texto('span', 'matricula', f.matricula));
        faltantes.appendChild(li);
      });
      $('vazio-faltantes').hidden = e.faltantes.length > 0;
    } else {
      // Sem a lista da turma o relatório é a fila de crachás lidos, sem nome
      // e sem faltantes — os nomes entram no envio (R3, HU-32).
      $('conta-presentes').textContent = '(' + e.sem_nome.length + ')';
      e.sem_nome.forEach(function (s) {
        var li = document.createElement('li');
        li.appendChild(texto('span', 'nome', s.matricula ? 'matrícula ' + s.matricula : 'crachá ' + s.uid));
        if (s.origem === 'manual') li.appendChild(texto('span', 'marca-manual', 'manual'));
        li.appendChild(texto('span', 'hora', hora(s.timestamp)));
        presentes.appendChild(li);
      });
      $('vazio-presentes').hidden = e.sem_nome.length > 0;
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

  mostrar('login');

})();
