// Rodar: g++ -std=c++17 -o test_deterministic main_deterministic.cpp ../core/src/Board.cpp ../core/src/BoardGenerator.cpp ../core/src/DeterministicSolver.cpp
// ./test_deterministic
#include <iostream>
#include <queue>
#include <set>
#include "../core/include/Board.h"
#include "../core/include/BoardGenerator.h"
#include "../core/include/DeterministicSolver.h"

using namespace ms;

void printBoard(const Board& board) {
    for (int y = 0; y < board.height(); y++) {
        for (int x = 0; x < board.width(); x++) {
            const Cell& cell = board.at(x, y);
            if (cell.state == CellState::Covered) {
                std::cout << " # ";
            } else if (cell.state == CellState::Flagged) {
                std::cout << " F ";
            } else if (cell.isMine) {
                std::cout << " * "; // só aparece se revelou mina por engano (não deveria acontecer)
            } else if (cell.adjacentMines == 0) {
                std::cout << " . ";
            } else {
                std::cout << " " << cell.adjacentMines << " ";
            }
        }
        std::cout << "\n";
    }
}

// Revela a célula clicada. Se adjacentMines == 0, propaga (flood fill) revelando
// os vizinhos em cascata até encontrar células numeradas (comportamento clássico
// de campo minado, condizente com o que voce observou nos comentários do gerador).
void revealCell(Board& board, int startX, int startY) {
    std::queue<std::pair<int,int>> toReveal;
    toReveal.push({startX, startY});

    while(!toReveal.empty()) {
        auto [x, y] = toReveal.front();
        toReveal.pop();

        Cell& cell = board.at(x, y);
        if(cell.state != CellState::Covered) continue; // já revelada, evita loop infinito

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

// Monta a lista de frontierNumbers: células reveladas que têm pelo menos
// uma vizinha ainda coberta (são essas que carregam informação útil pro solver).
std::vector<std::pair<int,int>> buildFrontier(const Board& board) {
    std::vector<std::pair<int,int>> frontier;
    for(int y = 0; y < board.height(); y++) {
        for(int x = 0; x < board.width(); x++) {
            if(board.at(x, y).state != CellState::Revealed) continue;
            if(board.at(x, y).adjacentMines == 0) continue; // célula "." não carrega restrição

            for(auto [nx, ny] : board.neighbors(x, y)) {
                if(board.at(nx, ny).state == CellState::Covered) {
                    frontier.push_back({x, y});
                    break;
                }
            }
        }
    }
    return frontier;
}

int main() {
    int width = 9;
    int height = 9;
    int mineCount = 10;
    int safeX = 4;
    int safeY = 4;

    Board board = BoardGenerator::generate(width, height, mineCount, safeX, safeY);

    std::cout << "=== Tabuleiro gerado (todas cobertas) ===\n";
    printBoard(board);

    // simula o primeiro clique
    revealCell(board, safeX, safeY);

    std::cout << "\n=== Apos o primeiro clique em (" << safeX << "," << safeY << ") ===\n";
    printBoard(board);

    std::vector<std::pair<int,int>> frontierNumbers = buildFrontier(board);

    std::cout << "\nfrontierNumbers (" << frontierNumbers.size() << " celulas): ";
    for(auto [x, y] : frontierNumbers) std::cout << "(" << x << "," << y << ") ";
    std::cout << "\n";

    DeterministicSolver solver;
    DeductionResult result = solver.solve(board, frontierNumbers);

    std::cout << "\n=== Resultado do solve ===\n";
    std::cout << "safeCells: ";
    for(auto [x, y] : result.safeCells) std::cout << "(" << x << "," << y << ") ";
    std::cout << "\nmineCells: ";
    for(auto [x, y] : result.mineCells) std::cout << "(" << x << "," << y << ") ";
    std::cout << "\n";

    std::cout << "\n=== Tabuleiro apos o solve (minas deduzidas viram F) ===\n";
    printBoard(board);

    // valida: nenhuma mina deduzida errado (comparando com isMine, que é o gabarito real)
    bool ok = true;
    for(auto& [x, y] : result.mineCells) {
        if(!board.at(x, y).isMine) {
            std::cout << "ERRO: (" << x << "," << y << ") foi marcada como mina mas nao e mina!\n";
            ok = false;
        }
    }
    for(auto& [x, y] : result.safeCells) {
        if(board.at(x, y).isMine) {
            std::cout << "ERRO: (" << x << "," << y << ") foi marcada como segura mas E mina!\n";
            ok = false;
        }
    }

    std::cout << (ok ? "\nNenhuma contradicao encontrada (deducoes batem com o gabarito real)\n"
                      : "\nENCONTROU CONTRADICAO -- tem bug\n");

    return ok ? 0 : 1;
}