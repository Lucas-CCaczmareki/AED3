#pragma once
#include "Board.h"
#include <vector>
#include <utility>
#include <PairHash.h>

namespace ms {

struct Frontier {
    std::vector<std::pair<int,int>> frontierCells;   // celulas cobertas adjacentes a numeros
    std::vector<std::pair<int,int>> frontierNumbers; // numeros revelados com vizinha coberta
};

class FrontierExtractor {
public:
    //static aqui permite que esse método seja utilizado mesmo
    //sem haver uma instância/objeto da classe FrontierExtractor
    static Frontier extract(const Board& board);
};

} // namespace ms
