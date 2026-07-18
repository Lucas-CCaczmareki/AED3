#pragma once
#include "Board.h"

namespace ms {

class BoardGenerator {
public:
    // Gera tabuleiro width x height com mineCount minas, garantindo
    // que (safeX, safeY) e vizinhas fiquem livres (regra do 1o clique seguro).
    static Board generate(int width, int height, int mineCount, int safeX, int safeY);
};

} // namespace ms
