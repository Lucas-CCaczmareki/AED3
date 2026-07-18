#include "BruteForceSolver.h"

namespace ms {

std::unordered_map<std::pair<int,int>, double, PairHash>
BruteForceSolver::solve(const Board& board, const Frontier& frontier) {
    std::unordered_map<std::pair<int,int>, double, PairHash> result;

    // TODO: enumerar todas as 2^n combinacoes da fronteira, filtrar validas,
    // contar frequencia exata de bomba por celula.

    // descarta os valores aqui só pra conseguir compilar certinho, dps tira esses (void)
    (void)board;
    (void)frontier;
    return result;
}

} // namespace ms
