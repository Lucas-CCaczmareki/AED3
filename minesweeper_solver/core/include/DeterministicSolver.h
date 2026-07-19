#pragma once
#include "Board.h"
#include "PairHash.h"
#include <iostream>
#include <vector>
#include <utility>
#include <unordered_map>
#include <stdexcept>
#include <algorithm>

namespace ms {

struct DeductionResult {
    std::vector<std::pair<int, int>> safeCells; // 100% seguras
    std::vector<std::pair<int, int>> mineCells;  // 100% bomba
};

class DeterministicSolver {
public:
    // Aplica regras deterministicas ate estabilizar. Retorna o que foi deduzido.
    static DeductionResult solve(Board& board, const std::vector<std::pair<int,int>>& frontierNumbers);

private:
    // hash de celulas satisfeitas, usada pra verificaçao
    std::unordered_map<std::pair<int, int>, bool, PairHash> satisfiedCells;
};

} // namespace ms
