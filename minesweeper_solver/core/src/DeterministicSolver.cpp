#include "../include/DeterministicSolver.h"

namespace ms {

DeductionResult DeterministicSolver::solve(Board& board, const std::vector<std::pair<int,int>>& frontierNumbers) {
    DeductionResult result;
    DeterministicSolver solver;
    bool stuck = false; // verificador pra saber se tem mais mudanças pra fazer ou se já fivou preso
    //int passada = 1; debug

    while(!stuck) {
        //std::cout << "Passada: " << passada << "\n";
        //passada++;
        stuck = true;
        for(auto [jx, iy] : frontierNumbers) {
            
            // se é uma celula já satisfeita, ignora e passa pra próxima
            if(solver.satisfiedCells[{jx, iy}]) continue;
            
            // conta a quantidade de celulas cobertas e a de bombas/minas (em codigo eu prefiro minas mas eu penso bombas e pra escrever eu sempre acabo escrevendo bombas apago e escrevo minas ent em comentarios vai ser bombas) em volta de uma célula da fronteira, ignorando as celulas já marcadas como seguras
            int countCovered = 0;
            int countMines = 0;
            for(auto [nx, ny] : board.neighbors(jx, iy)) {
                // aqui uma verificação pra ignorar as q estão marcadas como seguras, pq se não elas iriam contar como possiveis lugares para bombas
                std::pair<int, int> neighbor = {nx, ny};
                auto findSafe = std::find(result.safeCells.begin(), result.safeCells.end(), neighbor);
                bool isSafe = findSafe != result.safeCells.end();
                
                if(!isSafe && board.at(nx, ny).state == CellState::Covered) countCovered++;
                if(board.at(nx, ny).state == CellState::Flagged) countMines++;
            }
            
            // se todas as celulas ja tiverem ou sido marcadas como bombas ou como seguras ent marca a celula testada como satisfeita e passa pra proxima
            if(countCovered == 0) {
                solver.satisfiedCells[{jx, iy}] = true;
                continue;
            }

            int remainingMines = board.at(jx, iy).adjacentMines - countMines; // bombas restantes
            if(countCovered < remainingMines) {
                throw std::runtime_error("Quantidade de bombas maior q a de celulas nao descobertas ao redor");
            }

            // se n sobrar bombas restantes ent quer dizer q é uma celula ja satisfeita, o q diferencia do if do countCovered é q aqui é q se marca as cobertas q estao ao redor como seguras, colocando elas no vetor de seguras
            bool isSatisfied = remainingMines == 0;
            if(isSatisfied) {
                solver.satisfiedCells[{jx, iy}] = true;
                for(auto [nx, ny] : board.neighbors(jx, iy)){
                    // se tem mais celulas cobertas, colocar no safeCells
                    if(board.at(nx, ny).state == CellState::Covered) {
                        result.safeCells.emplace_back(nx, ny);
                    }
                }
                stuck = false; // marca q n ficou preso, q essa mudança das celulas seguras pode impactar nas celulas da fronteira ao redor
                continue;
            } 

            // se a quantidade de bombas restante for igual a de celulas cobertas ent marca a celula como satisfeita e marca as bombas
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
                stuck = false; // aqui como colocou mais bombas isso tbm pode impactar as celulas da fronteira ao redor ent marca q n ficou preso, pra verificar mais uma vez
            }

            // se countCovered > remainingMines, ou seja, passou por todos os ifs e n caiu em nenhum, quer dizer q esta preso e n tem mais o q fazer

        }
    }
    return result;
}

} // namespace ms

/*
Primeiras anotações: * significa q mudou um pouco

Feito em mais de uma passada, vai corrigindo a cada passada

- contagem pra cada célula da quantidade de bombas ao redor que já foram "descobertas"(estado Flagged: q é um estado da celula pra dizer q marcou como bomba) = coutBombs
- * se countBombs for igual ao numero de bombas adjacentes daquela celula(* ficou como se n tem bombas restantes remainingMines ), ent marca no vetor satisfiedCells como true e se tiver celulas cobertas ao redor, coloca elas no vetor de safeCells e marca q n ficou preso, q essa alteraçao pode ter mexido com as celulas ao redor
- * se countBombs < quantidade de celulas não descobertas ao redor(sem contar as celulas no safeCells), ent marca no vetor satisfiedCells como false e continua testando outras células, se for igual marca no vetor satisfiedCells como true e troca o estado da/s celula/s coberta/s(Covered) para Flagged, se for maior ent é erro pq n tem como acontecer 
- se a celula estiver satirfeita(satisfiedCells = true) passa reto, n faz nada

*/