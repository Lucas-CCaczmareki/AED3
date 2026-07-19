#pragma once
#include "Board.h"
#include "FrontierExtractor.h"
#include "PairHash.h" // reaproveita PairHash
#include "MonteCarloSolver.h" //reaproveita a struct contraint
#include <unordered_map>
#include <utility>
#include <vector>
#include <random>
#include <algorithm>

namespace ms {

//Cria uma estrutura pra conseguir representar as restrições (ex: A + B = 1) e facilitar o cálculo
// struct Constraint {
//     int target;                                 //número revelado, no exemplo acima target = 1
//     std::vector<std::pair<int, int>> variables; //a soma de cada variável aqui tem que ser = a target
// };

class BruteForceSolver {
public:
    /*
        O compilador padrão do C++ não sabe gerar um número de hash a partir da chave pair<int, int> (que usamos pra representar coordenada)
        por isso, criamos uma função que faz o número de hash(chave) desse par e enviamos como parâmetro pra hashtable do C.
    */
    // Testa TODAS as 2^n disposicoes possiveis da fronteira.
    std::unordered_map<std::pair<int,int>, double, PairHash> solve(const Board& board, const Frontier& frontier);
    bool wasInterruptedByTimeout() const { return timeoutOccurred_; }

private:
    bool timeoutOccurred_ = false;
    std::vector<std::pair<int, int>> orderedVariables_; //variáveis ordenadas pra dar de entrada no backtracking. Estarem ordenadas já melhora a chance de podar ramos ruins no começo
    std::vector<Constraint> constraints_;               //vetor com as restrições dos números revelados (FrontierNumbers)

    //vetor de "vetor" de pares que mapeiam pra [0, 1]. 
    std::vector<std::unordered_map<std::pair<int, int>, int, PairHash>> validSolutions_;

    //map q guarda um vetor com todos índices das constraints de uma variável
    std::unordered_map<std::pair<int, int>, std::vector<size_t>, PairHash> variableConstraints_;
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
    void backtrack(size_t index, std::unordered_map<std::pair<int, int>, int, PairHash>& currAssignments, std::chrono::high_resolution_clock::time_point endTime);

    // Confere se a atribuição atual quebrou alguma restrição
    bool violatesConstraints(const std::pair<int, int>& lastAssigned, const std::unordered_map<std::pair<int, int>, int, PairHash>& currAssignments);
};

} // namespace ms
