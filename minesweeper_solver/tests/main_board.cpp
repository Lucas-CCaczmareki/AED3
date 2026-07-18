// Rodar: g++ -std=c++17 -o test_board main_board.cpp ../core/src/Board.cpp ../core/src/BoardGenerator.cpp 
// ./test_board 
#include <iostream>
#include "../core/include/Board.h"
#include "../core/include/BoardGenerator.h"
 
using namespace ms;
 
void printBoard(const Board& board) {
    for (int y = 0; y < board.height(); y++) {
        std::cout << "y = " << y << "-> ";
        for (int x = 0; x < board.width(); x++) {
            std::cout << "x = " << x << " ";
            const Cell& cell = board.at(x, y);
            if (cell.isMine) {
                std::cout << "(*) ";
            } else if (cell.adjacentMines == 0) {
                std::cout << "(.) ";
            } else {
                std::cout << "(" << cell.adjacentMines << ") ";
            }
            std::cout << "| ";
        }
        std::cout << "\n";
    }
}

int main() {
    int width = 9;
    int height = 9;
    int mineCount = 10;
    int safeX = 4;
    int safeY = 4;
 
    Board board = BoardGenerator::generate(width, height, mineCount, safeX, safeY);
    printBoard(board);
 
    return 0;
}
