#pragma once
#include "Board.h"
#include <vector>
#include <utility>

namespace ms {

struct Frontier {
    std::vector<std::pair<int,int>> frontierCells;  // celulas cobertas adjacentes a numeros
    std::vector<std::pair<int,int>> frontierNumbers; // numeros revelados com vizinha coberta
};

class FrontierExtractor {
public:
    static Frontier extract(const Board& board);
};

} // namespace ms
