# Trabalho Final — AEDIII
## Campo Minado: Estimativa Probabilística de Bombas via Monte Carlo

**Disciplina:** Algoritmos e Estruturas de Dados III — UFPEL
**Opção escolhida:** Trabalho Prático (Opção 2)
**Ferramental:** C++ / Qt (interface) / CMake

**Alunos:** Lucas C. Caczmareki, Celina Gonçalves Lemos
**Professor:** Ulisses Brisolara Corrêa

---

# Setup

## Pra rodar no VSCode
**Pré-requisitos:** compilador C++, CMake, extensões C/C++ e CMake Tools

**No Windows:** além disso, precisa ter o [Git para Windows](https://git-scm.com/download/win)
instalado (o instalador padrão já adiciona `sh.exe` ao PATH automaticamente). O `make` no
Windows depende desse `sh.exe` pra interpretar os comandos do Makefile — sem ele, `make build`
falha. Não precisa necessariamente rodar o terminal Git Bash (MINGW64); qualquer terminal
(PowerShell, cmd, VSCode integrado) funciona, desde que o Git esteja instalado e no PATH.

**Depois:**
```bash
make run
```
Isso builda o projeto inteiro e roda `benchmark` + `tests`.

### Outros targets disponíveis

O `Makefile` também tem targets separados pra rodar cada análise estatística individualmente
(útil pra não esperar a bateria toda toda vez que só quer conferir um resultado específico):

```bash
make run-autoplayer      # joga N partidas completas e mede taxa de vitoria/derrota
make run-benchmark-mc-vs-bf # gera vários tabuleiros e compara desempenho do MC contra o BF
```

Se quiser rodar tudo de uma vez (demora mais, já que inclui os testes de maior volume):
```bash
make run-all
```

Pra limpar a pasta `build/` e forçar recompilação do zero:
```bash
make clean
```

## Pra rodar no Qt Creator
**Pré-requisitos:** Qt 6 (com Qt Widgets) + Qt Creator instalados

**Depois:**
1. `File → Open File or Project` → selecionar o `CMakeLists.txt` da raiz
2. Escolher o Kit do Qt6
3. Selecionar target `minesweeper_gui` → Run


## 1. Ideia geral do trabalho

O Campo Minado (Minesweeper) é comprovadamente **NP-completo** (Kaye, 2000): dado um
tabuleiro parcialmente revelado, decidir se uma configuração de bombas é consistente com
os números mostrados é um problema tão difícil quanto SAT. Isso significa que não existe
(até onde se sabe) um algoritmo eficiente que resolva qualquer tabuleiro de forma exata em
tempo razoável conforme ele cresce.

Por isso, ao invés de tentar resolver o tabuleiro com certeza absoluta o tempo todo, a
proposta é:

1. Resolver com lógica determinística tudo que der pra resolver com 100% de certeza
   utilizando regras e lógicas simples de dedução. Estamos arbitrariamente evitando
   usar regras muito avançadas de dedução (que um jogador experiente poderia usar) pois 
   isso faria sobrar menos casos pro Monte Carlo, que é o objetivo do trabalho. Porém, isso 
   já é muito rápido e cobre boa parte do jogo.

   **Complexidade**: O(n_números × 8 × iterações), sendo:
   *n_números:* números revelados que ainda têm ao menos uma vizinha coberta (fronteira ativa)
   *8:* quantidade máxima de células adjacentes a cada número
   *iterações:* toda vez que uma célula é resolvida com certeza (0% bomba ou 100% bomba), o processo roda de novo
   porque essa nova informação pode liberar novas deduções certas em números vizinhos. 

2. Quando a lógica determinística travar (nenhuma célula 100% segura ou 100% bomba),
   usar uma abordagem probabilística por amostragem (Monte Carlo) pra estimar a
   probabilidade de cada célula coberta conter uma bomba, e jogar na célula mais segura.
   
   **Complexidade**: O(k × n_num_fronteira × 8), sendo:
   *k:* número de amostras (disposições) geradas. Parâmetro escolhido arbitrariamente.
   *n_num_fronteira:* quantidade de números na fronteira que precisam ser checados
   *8:* quantidade máxima de células adjacentes a cada número

   O Monte Carlo funciona assim: gera-se uma disposição aleatória de bomba/não-bomba
   para as células cobertas da fronteira, verifica-se se ela é **consistente** (bate
   com todos os números da fronteira) e, se sim, ela é guardada como uma "amostra
   válida"; senão, é descartada. Esse processo se repete `k` vezes. No final, a
   probabilidade de bomba de cada célula é estimada contando, **entre as amostras válidas** 
   (não todas as `k` geradas, só as que sobraram depois do filtro), em
   quantas delas aquela célula específica tinha bomba, e dividindo pelo total de
   amostras válidas. Ou seja, o Monte Carlo não gera *todas* as combinações possíveis,
   ele gera só uma amostra, valida essa amostra, e estima a probabilidade real a
   partir dela. Por isso o resultado é uma **aproximação**, não um valor exato: quanto
   maior o `k`, mais a estimativa tende a se aproximar do valor real, mas nunca há
   garantia de exatidão.

3. Abordagem complementar com bruteforce geraria **todas** as disposições possíveis de 
   bomba/não-bomba para as células cobertas da fronteira (não uma amostra, o total
   exaustivo), verificaria a consistência de cada uma delas com os números, e
   calcularia a probabilidade **exata** de cada célula contando entre todas as
   disposições válidas encontradas, que aqui são o total real, não uma estimativa.
   É o mesmo processo de "gerar → validar → contar" do Monte Carlo, só que sem pular
   nenhuma combinação possível. É exatamente por gerar tudo que o custo dele explode
   exponencialmente, enquanto o Monte Carlo mantém o custo controlado por `k`.

   **Complexidade**: O(2^n_células_fronteira × n_números_fronteira × 8), sendo:
   *2:* cada opção de valor pra célula não revelada (bomba/não-bomba)
   *n_células_fronteira:* quantidade de células cobertas da fronteira (as que recebem bomba/não-bomba)
   *n_num_fronteira:* quantidade de números na fronteira que precisam ser checados
   *8:* quantidade máxima de células adjacentes a cada número

Ou seja, ambos bruteforce e Monte Carlo possuem a mesma lógica. A troca é a 
complexidade: no bruteforce cresce exponencialmente (2^n), enquanto no Monte Carlo 
fica limitada por k. A sacada está numa propriedade estatística (relacionada à Lei dos 
Grandes Números / Teorema Central do Limite): o erro de uma estimativa por amostragem 
decresce proporcionalmente a 1/sqrt(k), ou seja, a precisão melhora muito mais devagar do 
que o esforço (k) necessário pra alcançá-la. Na prática, isso significa que não é 
preciso testar o espaço de busca inteiro (2^n) para obter uma estimativa próxima do 
valor real. É essa propriedade que viabiliza resolver um problema CSP intratável em tempo razoável, 
trocando exatidão por uma aproximação estatisticamente confiável.

O Campo Minado serve aqui como um **caso de teste simples e visual** para uma técnica: 
estimativa por amostragem em problemas de satisfação de restrições (CSP), que se aplica
a problemas reais bem mais complexos (robótica com sensores incertos, diagnóstico médico
com sintomas parciais, etc.).

Analogia: é como fazer um censo envolvendo as 8 bilhões de pessoas no mundo (caro, nunca terminaria)
ou fazer um censo com amostras de 1000 pessoas de cada país.

---

## 2. Arquitetura do projeto
(é só uma ideia, pode acabar não sendo seguida a risca e ser apagada nos próximos commits)

Separar a lógica (`core`) da interface (`gui`) é importante pois facilita testar e rodar 
testes sem precisar abrir janela nenhuma.

Ideia inicial de estrutura
```
minesweeper-solver/
├── CMakeLists.txt
├── core/                              # lógica pura, sem dependência de Qt (backend)
│   ├── Board.h / Board.cpp            # representação do tabuleiro
│   ├── BoardGenerator.h / .cpp        # gera tabuleiros 8x8, 16x16, 32x32
│   ├── DeterministicSolver.h / .cpp   # regras de certeza 100%/0%
│   ├── FrontierExtractor.h / .cpp     # separa fronteira do resto do tabuleiro
│   ├── MonteCarloSolver.h / .cpp      # geração de candidatos + estimativa de probabilidade
│   ├── BruteForceSolver.h / .cpp      # força bruta, só para tabuleiros pequenos (gabarito)
│   └── AutoPlayer.h / .cpp            # laço de jogo automático
├── gui/                                # Qt Widgets (frontend)
│   ├── MainWindow.h / .cpp
│   ├── BoardWidget.h / .cpp           # grid clicável, mostra % em cada célula
│   └── ControlsPanel.h / .cpp         # tamanho do tabuleiro, nº de simulações, iniciar auto-play
├── benchmark/
│   └── main.cpp                       # roda experimentos sem GUI, gera CSV com métricas
└── tests/
    └── ...                            # testes unitários (principalmente do MonteCarloSolver)
```

---

## 3. Conclusão
Explicações mais detalhadas das regras e particularidades de cada algoritmo serão explicadas no relatório final do trabalho.
O relatório será disponibilizado como pdf nesse repositório quando finalizarmos sua implementação. 