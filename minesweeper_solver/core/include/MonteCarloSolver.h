#pragma once
#include "Board.h"
#include "FrontierExtractor.h"
#include <unordered_map>
#include <utility>
#include <PairHash.h>

namespace ms {

class MonteCarloSolver {
public:
    // Retorna probabilidade estimada de bomba [0,1] para cada celula da fronteira
    static std::unordered_map<std::pair<int,int>, double, PairHash> estimate(const Board& board, const Frontier& frontier, int numSamples);
};

} // namespace ms
