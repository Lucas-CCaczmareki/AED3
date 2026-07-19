#include "../include/DeterministicSolver.h"

namespace ms {

DeductionResult DeterministicSolver::solve(Board& board, const std::vector<std::pair<int,int>>& frontierNumbers) {
    DeductionResult result;
    DeterministicSolver solver;
    bool stuck = false;
    int passada = 1;

    while(!stuck) {
        std::cout << "Passada: " << passada << "\n";
        passada++;
        stuck = true;
        for(auto [jx, iy] : frontierNumbers) {
            
            // se é uma celula já satisfeita, passa pra próxima
            if(solver.satisfiedCells[{jx, iy}]) continue;
            
            // contar a quantidade de celulas cobertas e a de minas em vonta de uma célula da fronteira
            int countCovered = 0;
            int countMines = 0;
            for(auto [nx, ny] : board.neighbors(jx, iy)) {
                std::pair<int, int> neighbor = {nx, ny};
                auto findSafe = std::find(result.safeCells.begin(), result.safeCells.end(), neighbor);
                bool isSafe = findSafe != result.safeCells.end();
                
                if(!isSafe && board.at(nx, ny).state == CellState::Covered) countCovered++;
                if(board.at(nx, ny).state == CellState::Flagged) countMines++;
            }
            
            if(countCovered == 0) {
                solver.satisfiedCells[{jx, iy}] = true;
                continue;
            }

            int remainingMines = board.at(jx, iy).adjacentMines - countMines;
            if(countCovered < remainingMines) {
                throw std::runtime_error("Quantidade de bombas maior q a de celulas nao descobertas ao redor");
            }

            bool isSatisfied = remainingMines == 0;
            if(isSatisfied) {
                solver.satisfiedCells[{jx, iy}] = true;
                for(auto [nx, ny] : board.neighbors(jx, iy)){
                    // se tem mais celulas cobertas, colocar no safeCells
                    if(board.at(nx, ny).state == CellState::Covered) {
                        result.safeCells.emplace_back(nx, ny);
                    }
                }
                stuck = false; 
                continue;
            } 

            if(countCovered == remainingMines) {
                solver.satisfiedCells[{jx, iy}] = true;
                for(auto [nx, ny] : board.neighbors(jx, iy)) {
                    std::pair<int, int> neighbor = {nx, ny};
                    auto findSafe = std::find(result.safeCells.begin(), result.safeCells.end(), neighbor);
                    bool isSafe = findSafe != result.safeCells.end();

                    if(!isSafe && board.at(nx, ny).state == CellState::Covered) {
                        board.at(nx, ny).state = CellState::Flagged;
                        result.mineCells.emplace_back(nx, ny);
                    }
                }
                stuck = false;
            }

            // se countCovered > remainingMines, ou seja, passou por todos os ifs e n caiu em nenhum, quer dizer q esta preso e n tem mais o q fazer

        }
    }
    return result;
}

} // namespace ms
