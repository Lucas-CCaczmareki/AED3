#include "BoardGenerator.h"

namespace ms {

Board BoardGenerator::generate(int width, int height, int mineCount, int safeX, int safeY) {
    Board board(width, height);
    // TODO: sortear minas evitando (safeX, safeY) e vizinhas,
    // depois calcular adjacentMines de cada célula.
    (void)mineCount;
    (void)safeX;
    (void)safeY;
    return board;
}

} // namespace ms
