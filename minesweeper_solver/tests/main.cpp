#include <iostream>
#include "Board.h"
#include "FrontierExtractor.h"
 
using namespace ms;
 
// Monta um tabuleiro 4x4 na mao, com estado fixo, so pra testar o
// FrontierExtractor sem depender do BoardGenerator (que a dupla ainda
// nao terminou).
//
// Layout (R = revelado com numero, # = coberto):
//   x:   0  1  2  3
//   y=0: #  #  #  #
//   y=1: #  1  1  #
//   y=2: #  1  #  #
//   y=3: #  #  #  #
Board buildManualBoard() {
    Board board(4, 4);
 
    // Por padrao todas as celulas ja nascem Covered (ver Board.h),
    // entao so precisamos "revelar" as que queremos que tenham numero.
 
    // (1,1) = numero 1
    board.at(1, 1).state = CellState::Revealed;
    board.at(1, 1).adjacentMines = 1;
 
    // (2,1) = numero 1
    board.at(2, 1).state = CellState::Revealed;
    board.at(2, 1).adjacentMines = 1;
 
    // (1,2) = numero 1
    board.at(1, 2).state = CellState::Revealed;
    board.at(1, 2).adjacentMines = 1;
 
    // (2,2) fica Covered de propósito -- é a única célula realmente
    // "encurralada" entre números nesse layout pequeno.
 
    return board;
}
 
int main() {
    Board board(8, 8);

    if (board.width() == 8 && board.height() == 8) {
        std::cout << "OK: Board criado com dimensoes corretas.\n";
    } else {
        std::cerr << "FALHOU: dimensoes incorretas.\n";
        return 1;
    }
 
    std::cout << "\n--- Teste manual do FrontierExtractor (tabuleiro 4x4) ---\n";
    Board manual = buildManualBoard();
    Frontier f = FrontierExtractor::extract(manual);
 
    std::cout << "frontierNumbers (" << f.frontierNumbers.size() << "):\n";
    for (auto& p : f.frontierNumbers) {
        std::cout << "  (" << p.first << "," << p.second << ")\n";
    }
 
    std::cout << "frontierCells (" << f.frontierCells.size() << "):\n";
    for (auto& p : f.frontierCells) {
        std::cout << "  (" << p.first << "," << p.second << ")\n";
    }
 
    return 0;
}
 