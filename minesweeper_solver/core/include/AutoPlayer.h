#pragma once
#include "Board.h"

namespace ms {

enum class GameResult { Ongoing, Won, Lost };

class AutoPlayer {
public:
    // Joga uma partida inteira automaticamente. Retorna o resultado final.
    static GameResult playGame(Board& board, int monteCarloSamples);
};

} // namespace ms
