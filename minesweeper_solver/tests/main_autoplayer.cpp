// Rodar: g++ -std=c++17 -o test_autoplayer main_autoplayer.cpp ../core/src/Board.cpp ../core/src/BoardGenerator.cpp ../core/src/DeterministicSolver.cpp ../core/src/MonteCarloSolver.cpp ../core/src/FrontierExtractor.cpp ../core/src/BruteForceSolver.cpp
// ./test_autoplayer
#include <iostream>
#include "../core/include/Board.h"
#include "../core/include/BoardGenerator.h"
#include "../core/include/AutoPlayer.h"
 
using namespace ms;
 
int main() {
    int width = 18, height = 14, mineCount = 40, safeX = 14, safeY = 5;
    int wins = 0, losses = 0;
    int rounds = 60;
 
    for(int i = 0; i < rounds; i++) {
        Board board = BoardGenerator::generate(width, height, mineCount, safeX, safeY);
        AutoPlayer::firstClick(board, safeX, safeY);
        GameResult r = AutoPlayer::playGame(board, 200);
 
        if(r == GameResult::Won) { wins++; std::cout << "Rodada " << i << ": VENCEU\n"; }
        else if(r == GameResult::Lost) { losses++; std::cout << "Rodada " << i << ": PERDEU\n"; }
        else std::cout << "Rodada " << i << ": ONGOING (nao deveria terminar assim)\n";
    }
 
    std::cout << "\nResumo: " << wins << " vitorias, " << losses << " derrotas, de " << rounds << " rodadas\n";
    return 0;
}