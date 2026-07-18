#pragma once
#include "Board.h"
#include "FrontierExtractor.h"
#include <unordered_map>
#include <utility>

namespace ms {

// hash simples pra pair<int,int> ser chave de unordered_map
struct PairHash {
    size_t operator()(const std::pair<int,int>& p) const {
        return std::hash<long long>()((static_cast<long long>(p.first) << 32) ^ p.second);
    }
};

class MonteCarloSolver {
public:
    // Retorna probabilidade estimada de bomba [0,1] para cada celula da fronteira
    static std::unordered_map<std::pair<int,int>, double, PairHash>
    estimate(const Board& board, const Frontier& frontier, int numSamples);
};

} // namespace ms
