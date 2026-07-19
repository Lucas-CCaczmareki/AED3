#include <iostream>
#include <iomanip>
#include <vector>
#include "Board.h"
#include "FrontierExtractor.h"
#include "MonteCarloSolver.h"
#include "BruteForceSolver.h"
#include "BoardGenerator.h"
#include "DeterministicSolver.h"
 
using namespace ms;
 
// Simula o "primeiro clique" de verdade: revela a celula clicada e, se ela
// for 0 (sem bombas vizinhas), propaga a revelacao em cascata pelas vizinhas
// (exatamente como o Minesweeper de verdade faz quando voce clica numa area
// vazia). Isso avanca o estado do board de "recem-gerado" pra "em progresso".
void revealCascade(Board& board, int startX, int startY) {
    std::vector<std::pair<int,int>> stack;
    stack.push_back({startX, startY});
 
    while (!stack.empty()) {
        auto [x, y] = stack.back();
        stack.pop_back();
 
        Cell& c = board.at(x, y);
        if (c.state == CellState::Revealed) continue;
        if (c.isMine) continue; // seguranca -- nao deveria acontecer na safe zone
 
        c.state = CellState::Revealed;
 
        if (c.adjacentMines == 0) {
            for (auto [nx, ny] : board.neighbors(x, y)) {
                if (board.at(nx, ny).state != CellState::Revealed) {
                    stack.push_back({nx, ny});
                }
            }
        }
    }
}
 
void printBoard(const Board& board) {
    for (int y = 0; y < board.height(); y++) {
        for (int x = 0; x < board.width(); x++) {
            const Cell& cell = board.at(x, y);
            if (cell.state == CellState::Covered) {
                std::cout << " # ";
            } else if (cell.isMine) {
                std::cout << " * ";
            } else if (cell.adjacentMines == 0) {
                std::cout << " . ";
            } else {
                std::cout << " " << cell.adjacentMines << " ";
            }
        }
        std::cout << "\n";
    }
}
 
int main() {
    int width = 64;
    int height = 64;
    int mineCount = 1024;
    int safeX = 6;
    int safeY = 6;
 
    Board board = BoardGenerator::generate(width, height, mineCount, safeX, safeY);
 
    std::cout << "--- Tabuleiro" << width << "x" << height << "recem-gerado (antes do 1o clique) ---\n";
    printBoard(board);
 
    std::cout << "\n--- Simulando o 1o clique em (" << safeX << "," << safeY << ") ---\n";
    revealCascade(board, safeX, safeY);
    printBoard(board);
 
    Frontier f = FrontierExtractor::extract(board);
    std::cout << "\nfrontierNumbers: " << f.frontierNumbers.size()
              << " | frontierCells: " << f.frontierCells.size() << "\n";
 
    if (f.frontierCells.empty()) {
        std::cout << "Fronteira vazia (cascata cobriu o tabuleiro todo ou nao sobrou nada"
                     " incerto) -- tenta rodar de novo, o resultado do gerador e aleatorio.\n";
        return 0;
    }

    // --- Loop externo: roda o DeterministicSolver quantas vezes forem necessarias,
    // revelando/marcando e recalculando a fronteira, ate parar de conseguir deduzir algo novo ---
    DeterministicSolver detSolver;
    bool changed = true;
    int rodada = 1;

    while (changed) {
        changed = false;

        DeductionResult result = detSolver.solve(board, f.frontierNumbers);

        std::cout << "\n--- Rodada " << rodada << " do DeterministicSolver ---\n";
        std::cout << "Minas encontradas: " << result.mineCells.size()
                  << " | Seguras encontradas: " << result.safeCells.size() << "\n";
        rodada++;

        // Revela as celulas que o solver garantiu que sao seguras
        for (auto [sx, sy] : result.safeCells) {
            if (board.at(sx, sy).state == CellState::Covered) {
                revealCascade(board, sx, sy);
                changed = true;
            }
        }

        // Minas encontradas tambem contam como "mudanca", mesmo sem revelar nada,
        // porque isso pode alterar contas de countMines nas proximas rodadas
        if (!result.mineCells.empty()) {
            changed = true;
        }

        // Recalcula a fronteira porque o board mudou
        f = FrontierExtractor::extract(board);

        std::cout << "Fronteira apos rodada -> Numeros: " << f.frontierNumbers.size()
                  << " | Celulas: " << f.frontierCells.size() << "\n";

        if (f.frontierCells.empty()) {
            std::cout << "Tabuleiro completamente resolvido ou sem fronteiras pendentes!\n";
            printBoard(board);
            return 0;
        }
    }

    std::cout << "\n--- MonteCarloSolver (k=1000) no tabuleiro ---\n";
    MonteCarloSolver mc;
    auto probsMC = mc.estimate(board, f, 1000);
    std::cout << std::fixed << std::setprecision(4);
    for (auto& [cell, prob] : probsMC) {
        std::cout << "  (" << cell.first << "," << cell.second << ") = " << (prob * 100.0) << "%\n";
    }
    
    //descomentar limite de seguranca se achar necessario
    // if (f.frontierCells.size() <= 20) {
        std::cout << "\n--- BruteForceSolver (fronteira " << f.frontierCells.size() << " celulas) ---\n";
        BruteForceSolver bf;
        auto probsBF = bf.solve(board, f);
        for (auto& [cell, prob] : probsBF) {
            std::cout << "  (" << cell.first << "," << cell.second << ") = " << (prob * 100.0) << "%\n";
        }
    // } else {
    //     std::cout << "\n--- BruteForceSolver pulado: fronteira tem " << f.frontierCells.size()
    //               << " celulas, grande demais pra 2^n exaustivo ---\n";
    // }
 
    return 0;
}