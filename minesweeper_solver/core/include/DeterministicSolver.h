#pragma once
#include "Board.h"
#include <vector>
#include <utility>

namespace ms {

struct DeductionResult {
    std::vector<std::pair<int,int>> safeCells; // 100% seguras
    std::vector<std::pair<int,int>> mineCells;  // 100% bomba
};

class DeterministicSolver {
public:
    // Aplica regras deterministicas ate estabilizar. Retorna o que foi deduzido.
    static DeductionResult solve(Board& board);
};

} // namespace ms
