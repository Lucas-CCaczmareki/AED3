#pragma once
#include "Board.h"
#include "DeterministicSolver.h"
#include "FrontierExtractor.h"
#include "MonteCarloSolver.h"
#include <iostream>
#include <queue>
#include <algorithm>

namespace ms {
 
enum class GameResult { Ongoing, Won, Lost };
 
class AutoPlayer {
public:
    static GameResult playGame(Board& board, int monteCarloSamples);
    static void firstClick(Board& board, int startX, int startY);
 
private:
    // revela a celula e se tem umas com 0 bombas ao redor expande revelando em cascata
    static void revealCell(Board& board, int startX, int startY);
 
    // verifica se todas as celulas ja foram descobertas
    static bool allSafeCellsRevealed(const Board& board);
 
    // escolhe uma celula aleatoria se a fronteira ta travada
    static bool findAnyCovered(const Board& board, int& outX, int& outY);
};
 
} // namespace ms
