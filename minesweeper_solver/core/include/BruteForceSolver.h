#pragma once
#include "Board.h"
#include "FrontierExtractor.h"
#include "MonteCarloSolver.h" // reaproveita PairHash
#include <unordered_map>

namespace ms {

class BruteForceSolver {
public:

    /*
        O compilador padrão do C++ não sabe gerar um número de hash a partir da chave pair<int, int> (que usamos pra representar coordenada)
        por isso, criamos uma função que faz o número de hash desse par e enviamos como parâmetro pra hashtable do C.
    */

    // Testa TODAS as 2^n disposicoes possiveis da fronteira.
    static std::unordered_map<std::pair<int,int>, double, PairHash> solve(const Board& board, const Frontier& frontier);
};

} // namespace ms
