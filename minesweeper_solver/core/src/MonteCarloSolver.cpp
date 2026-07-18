#include "MonteCarloSolver.h"

namespace ms {

std::unordered_map<std::pair<int,int>, double, PairHash>
MonteCarloSolver::estimate(const Board& board, const Frontier& frontier, int numSamples) {
    std::unordered_map<std::pair<int,int>, double, PairHash> result;
    // TODO: gerar numSamples disposicoes (idealmente via backtracking com poda),
    // filtrar validas, contar frequencia de bomba por celula, dividir pelo total valido.

    // descarta os valores aqui só pra conseguir compilar certinho, dps tira esses (void)
    (void)board;
    (void)frontier;
    (void)numSamples;
    return result;
}

} // namespace ms
