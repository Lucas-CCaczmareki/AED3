#pragma once
#include "Board.h"
#include <vector>
#include <random>
#include <stdexcept>
#include <cmath>

namespace ms {

class BoardGenerator {
public:
    // Gera tabuleiro width x height com mineCount minas, garantindo
    // que (safeX, safeY) e vizinhas fiquem livres (regra do 1o clique seguro).
    static Board generate(int width, int height, int mineCount, int safeX, int safeY);

    //overload com seed opcional
    static Board generate(int width, int height, int mineCount, int safeX, int safeY, unsigned int seed);


private:
    static bool isSafeZone(int x, int y, int clickX, int clickY);
};

} // namespace ms
