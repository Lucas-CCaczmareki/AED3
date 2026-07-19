#include "../include/AutoPlayer.h"

namespace ms {

void AutoPlayer::firstClick(Board& board, int startX, int startY) {
    revealCell(board, startX, startY);
}
 
// revela a celula e se tem umas com 0 bombas ao redor expande revelando em cascata
void AutoPlayer::revealCell(Board& board, int startX, int startY) {
    std::queue<std::pair<int,int>> toReveal;
    toReveal.push({startX, startY});
 
    while(!toReveal.empty()) {
        auto [x, y] = toReveal.front();
        toReveal.pop();
 
        Cell& cell = board.at(x, y);
        if(cell.state != CellState::Covered) continue; // já revelada ou flagged evita loop infinito
 
        cell.state = CellState::Revealed;
 
        if(cell.adjacentMines == 0) {
            for(auto [nx, ny] : board.neighbors(x, y)) {
                if(board.at(nx, ny).state == CellState::Covered) {
                    toReveal.push({nx, ny});
                }
            }
        }
    }
}

// testar se todas foram reveladas, se sim entao é vitoria
bool AutoPlayer::allSafeCellsRevealed(const Board& board) {
    for(int y = 0; y < board.height(); y++) {
        for(int x = 0; x < board.width(); x++) {
            const Cell& cell = board.at(x, y);
            if(!cell.isMine && cell.state != CellState::Revealed) return false;
        }
    }
    return true;
}

// nao tinha nenhuma variável de fronteira pra estimar ent chuta
bool AutoPlayer::findAnyCovered(const Board& board, int& outX, int& outY) {
    for(int y = 0; y < board.height(); y++) {
        for(int x = 0; x < board.width(); x++) {
            if(board.at(x, y).state == CellState::Covered) {
                outX = x;
                outY = y;
                return true;
            }
        }
    }
    return false;
}

GameResult AutoPlayer::playGame(Board& board, int monteCarloSamples) {
    DeterministicSolver detSolver;
    MonteCarloSolver mcSolver;
    
    while(true) {
        Frontier frontier;
        bool progressed = true; // validaçao do deterministico
        
        // faz o deterministico até travar
        while (progressed) {
            // o deterministico pode vencer sozinho tbm, ent a verificação fica aqui
            if(allSafeCellsRevealed(board)) return GameResult::Won;
            
            frontier = FrontierExtractor::extract(board);
            progressed = false;
            
            if(!frontier.frontierNumbers.empty()) {
                DeductionResult result = detSolver.solve(board, frontier.frontierNumbers);
                progressed = !result.safeCells.empty() || !result.mineCells.empty();
                
                if(progressed) {
                    // as mineCells já saem Flagged de dentro do solve
                    for(auto& [x, y] : result.safeCells) {
                        if(board.at(x, y).state != CellState::Covered) continue; 
                        revealCell(board, x, y);
                        if(board.at(x, y).isMine) return GameResult::Lost; 
                    }
                }
            }
        }
        
        // se travou usa o montecarlo
        auto probabilities = mcSolver.estimate(board, frontier, monteCarloSamples);
        
        int chosenX, chosenY;
        bool hasChoice = false;
        
        if(!probabilities.empty()) {
            auto best = std::min_element(
                probabilities.begin(), probabilities.end(),
                [](const auto& a, const auto& b) { return a.second < b.second; }
            );
            chosenX = best->first.first;
            chosenY = best->first.second;
            hasChoice = true;
        } else {
            // as vezes a fronteira ta cercada de bombas e nenhuma celula segura livre pra clicar, ent tem q clicar em uma coberta aleatoriamente, se n o jogo trava 
            hasChoice = findAnyCovered(board, chosenX, chosenY);
        }
        
        // em caso de algo dar errado debug
        if(!hasChoice) {
            return GameResult::Ongoing;
        }
 
        revealCell(board, chosenX, chosenY);
        // se clicou em uma celula bomba perdeu
        if(board.at(chosenX, chosenY).isMine) return GameResult::Lost;
    }
}
 
} // namespace ms
