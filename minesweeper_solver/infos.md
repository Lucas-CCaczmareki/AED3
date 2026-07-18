## Divisão das tarefas para organização

## Tracking
- [ ] Passo 1
- [ ] Passo 2
- [X] Passo 3
- [ ] Passo 4
- [ ] Passo 5
- [ ] Passo 6
- [ ] Passo 7
- [ ] Passo 8
- [ ] Passo 9
- [ ] Relatório

(celina)
### Passo 1 — `Board` e `BoardGenerator`
- Representação do tabuleiro: matriz de células, cada uma com estado
  (coberta / revelada / marcada) e valor (número de bombas vizinhas, ou bomba).
- Função de vizinhança (8 vizinhos, tratando bordas).
- Geração aleatória de bombas respeitando a regra do primeiro clique seguro.
- Suporte a tamanhos 8x8, 16x16, 32x32 (parametrizável).

(celina)
### Passo 2 — `DeterministicSolver`
- Implementa as duas regras da seção 3.1.
- Aplica em loop até não conseguir mais deduzir nada com certeza.
- Retorna: lista de células marcadas como bomba, lista de células marcadas como seguras.

(lucas)
### Passo 3 — `FrontierExtractor`
- Dado o estado atual do tabuleiro, identifica quais células cobertas são fronteira
  (adjacentes a algum número revelado) e quais não são.

(celina)
### Passo 4 — `BruteForceSolver` (referência/gabarito)
- Só usado em tabuleiros pequenos (8x8, ou até menos, dependendo do tamanho da fronteira).
- Enumera exaustivamente todas as combinações possíveis de bomba/seguro na fronteira e
  calcula a probabilidade exata.
- Serve para **validar** se o Monte Carlo está calculando probabilidades corretas
  (comparar erro entre os dois).

(lucas)
### Passo 5 — `MonteCarloSolver`
- Implementa a geração de candidatos consistentes via backtracking restrito à fronteira
  (seção 3.3).
- Parâmetro: número de amostras/candidatos a gerar.
- Retorna probabilidade estimada de bomba para cada célula da fronteira.
- Para células fora da fronteira, usa a fórmula uniforme da seção 3.2.

(lucas)
### Passo 6 — `AutoPlayer`
- Loop principal:
  1. Roda `DeterministicSolver`.
  2. Se resolveu algo, revela/marca e repete o passo 1.
  3. Se travou, roda `FrontierExtractor` + `MonteCarloSolver`.
  4. Escolhe a célula com **menor** probabilidade de bomba e revela.
  5. Repete até vencer (todas as seguras reveladas) ou perder (clicou em bomba).

(lucas)
### Passo 7 — Interface Qt
- `BoardWidget`: grid clicável (via `QGridLayout` de botões, ou `QGraphicsView`
  customizado para melhor performance em 32x32). Mostra a % de bomba em cada célula
  coberta (calculada pelo mesmo `MonteCarloSolver`).
- `ControlsPanel`: escolher tamanho do tabuleiro, número de simulações do Monte Carlo,
  botão de "jogar automaticamente", botão de reiniciar.
- `MainWindow`: junta tudo.

(celina)
### Passo 8 — Benchmark (sem GUI)
- Programa separado (`benchmark/main.cpp`) que roda o `AutoPlayer` N vezes por tamanho de
  tabuleiro e gera um CSV com:
  - taxa de vitória por tamanho de tabuleiro,
  - tempo médio por jogada e por partida completa,
  - número de simulações Monte Carlo necessárias até a estimativa estabilizar
    (convergência),
  - comparação (erro médio) entre Monte Carlo e força bruta nos tabuleiros pequenos.
- Esses dados alimentam os gráficos/tabelas do relatório técnico.

(ambos)
### Passo 9 — Testes
- Testes unitários do `DeterministicSolver` (casos conhecidos com resultado esperado).
- Testes do `MonteCarloSolver` comparando com `BruteForceSolver` em tabuleiros pequenos
  (erro deve cair conforme aumenta o nº de amostras).