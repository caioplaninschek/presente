/* Presente! — configuração da página de histórico.
 *
 * Este arquivo é o exemplo versionado, e não tem credencial nenhuma. O
 * config.js de verdade nunca entra no git (R50): na publicação, quem o escreve
 * é o workflow do GitHub Actions, a partir dos segredos do repositório; na sua
 * máquina, copie este arquivo para `config.js` e preencha os dois valores.
 *
 * A chave é a publicável (`sb_publishable_…`, R35). Ela fica visível para quem
 * abrir a página — é o custo assumido na R50 —, e por isso sozinha ela não lê
 * nada: sem login ela vale como papel `anon`, que não tem acesso a tabela
 * alguma. Quem libera a leitura é o login (R34).
 */
window.CONFIG = {
  url: '',      // https://<projeto>.supabase.co
  chave: ''     // sb_publishable_...
};
