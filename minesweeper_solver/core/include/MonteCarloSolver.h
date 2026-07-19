#pragma once
#include "Board.h"
#include "FrontierExtractor.h"
#include "PairHash.h"
#include <unordered_map>
#include <utility>
#include <vector>
#include <random>
#include <algorithm> //pra trazer o sort 

/* Glossário (dps ajeita pra deixar legível)
Variável: cada célula coberta da fronteira é tratada como uma "caixinha" que só pode guardar dois valores possíveis: 0 (sem bomba) ou 1 (com bomba). Ainda não sabemos qual valor é o certo — é isso que estamos tentando descobrir.
Atribuição: o ato de decidir, num momento específico da busca, que uma variável vale 0 ou vale 1. Antes de atribuir, ela tá "em aberto"; depois de atribuir, ela tem um valor definido temporariamente (pode ser desfeito depois).
Restrição (ou Equação): uma regra vinda de um número revelado, dizendo "a soma de tais variáveis específicas precisa dar exatamente tal valor". Exemplo: A + B = 1.
Alvo da restrição: o número do lado direito da equação — o valor que a soma precisa bater. Em A + B = 1, o alvo é 1.
Violação: quando uma restrição é desrespeitada. Isso pode acontecer de duas formas:

A soma já passou do alvo antes mesmo de todas as variáveis serem decididas (ex: alvo é 1, mas já tem duas variáveis valendo 1 — impossível consertar depois).
Todas as variáveis da restrição já foram decididas, mas a soma final não bateu com o alvo.


Espaço de busca: o conjunto de todas as combinações possíveis de valores pras variáveis, sem considerar ainda se são válidas ou não. Com 3 variáveis, o espaço de busca tem 2³ = 8 combinações possíveis.
Árvore de busca: uma forma de organizar/visualizar o processo de tentar valores. Imagina literalmente uma árvore: cada "nível" da árvore representa decidir uma variável, e cada "galho" que sai dali representa um dos valores possíveis (0 ou 1) pra aquela variável.
Nó: um ponto específico dessa árvore — representa "até aqui, decidimos tais valores pra tais variáveis".
Ramo: o caminho que liga um nó a um dos seus filhos — representa a escolha de um valor específico pra variável atual.
Backtracking (retrocesso): a técnica de andar por essa árvore fazendo uma escolha, checando se ainda é válida, continuando se for válida, e — o ponto principal — voltando pro nó anterior e tentando o outro valor quando a escolha atual leva a um beco sem saída (violação).
Poda: desistir de explorar um ramo da árvore assim que se descobre que ele nunca vai dar certo, em vez de continuar preenchendo tudo até o fim só pra descobrir isso depois (o que seria desperdício).
Recursão / chamada recursiva: uma função que chama a si mesma pra resolver uma versão menor do mesmo problema. Aqui: "decide o valor da variável atual" termina chamando "decide o valor da próxima variável", que chama "decide o valor da variável depois dessa", e assim por diante.
Pilha de chamadas (call stack): o mecanismo que o computador usa pra "lembrar onde parou" em cada chamada recursiva ativa ao mesmo tempo. Pensa numa pilha de papéis: cada vez que você chama a próxima função, empilha um papel novo em cima. Quando aquela chamada termina (ou quando você faz backtrack), você tira o papel do topo e volta pra trabalhar no papel que ficou logo abaixo.
Caso base: a condição que faz a recursão parar. Aqui: "todas as variáveis já foram atribuídas".
Disposição válida (ou solução válida): um conjunto completo de valores (0/1) pra todas as variáveis, que não viola nenhuma restrição.
Amostra: no contexto do Monte Carlo, cada disposição válida encontrada é uma "amostra" que entra na contagem final.
k: o número de amostras válidas que vocês decidem coletar antes de parar a busca (parâmetro que vocês escolhem, tipo 1000).
Frequência / contagem: quantas vezes, entre todas as disposições válidas encontradas, uma variável específica apareceu valendo 1 (bomba).
*/

namespace ms {

//Cria uma estrutura pra conseguir representar as restrições (ex: A + B = 1) e facilitar o cálculo
struct Constraint {
    int target;                                 //número revelado, no exemplo acima target = 1
    std::vector<std::pair<int, int>> variables; //a soma de cada variável aqui tem que ser = a target
};

class MonteCarloSolver {
public:
    // Retorna probabilidade estimada de bomba [0,1] para cada celula da fronteira
    std::unordered_map<std::pair<int,int>, double, PairHash> 

    estimate(const Board& board, const Frontier& frontier, int numSamples);

private:
    int numSamples_;
    std::vector<std::pair<int, int>> orderedVariables_; //variáveis ordenadas pra dar de entrada no backtracking. Estarem ordenadas já melhora a chance de podar ramos ruins no começo
    std::vector<Constraint> constraints_;               //vetor com as restrições dos números revelados (FrontierNumbers)

    //vetor de "vetor" de pares que mapeiam pra [0, 1]. 
    std::vector<std::unordered_map<std::pair<int, int>, int, PairHash>> validSolutions_;

    //map q guarda um vetor com todos índices das constraints de uma variável
    std::unordered_map<std::pair<int, int>, std::vector<size_t>, PairHash> variableConstraints_;

    //gerador de números aleatórios. Vai garantir que a amostra fique mais aleatória e melhore os resultados
    std::mt19937 rng_; 

    // ---------------------------
    //     Funções auxiliares
    // ---------------------------

    // Função que cria as restrições pra uma célula;
    // varre frontierNumbers e descobre as variáveis (cobertas) que eles se referenciam
    void buildConstraints(const Board& board, const Frontier& frontier);

    // Ordena a lista de variáveis (cobertas) na fronteira pela incidência de aparecimento delas nas restrições
    void sortVariables(const Frontier& frontier);

    // currAssignmenst aqui é o vetor com as atribuições que já foram feitas no nível atual backtrack
    // Faz o backtracking recursivo com limite pra geração de amostras
    bool backtrack(size_t index, std::unordered_map<std::pair<int, int>, int, PairHash>& currAssignments);

    // Confere se a atribuição atual quebrou alguma restrição
    bool violatesConstraints(const std::pair<int, int>& lastAssigned, const std::unordered_map<std::pair<int, int>, int, PairHash>& currAssignments);
};

} // namespace ms
