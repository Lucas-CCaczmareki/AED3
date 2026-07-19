#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>
#include <chrono>
#include <numeric>
#include "Board.h"
#include "BoardGenerator.h"
#include "FrontierExtractor.h"
#include "DeterministicSolver.h"
#include "MonteCarloSolver.h"
#include "BruteForceSolver.h"
#include "AutoPlayer.h"

using namespace ms;
using Clock = std::chrono::high_resolution_clock;

void revealCascade(Board& board, int startX, int startY) {
    std::vector<std::pair<int,int>> stack;
    stack.push_back({startX, startY});

    while (!stack.empty()) {
        auto [x, y] = stack.back();
        stack.pop_back();

        Cell& c = board.at(x, y);
        if (c.state == CellState::Revealed) continue;
        if (c.isMine) continue;

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

Frontier stabilizeWithDeterministicSolver(Board& board) {
    Frontier f = FrontierExtractor::extract(board);
    DeterministicSolver detSolver;
    bool changed = true;

    while (changed && !f.frontierCells.empty()) {
        changed = false;
        DeductionResult result = detSolver.solve(board, f.frontierNumbers);

        for (auto [sx, sy] : result.safeCells) {
            if (board.at(sx, sy).state == CellState::Covered) {
                revealCascade(board, sx, sy);
                changed = true;
            }
        }
        if (!result.mineCells.empty()) {
            changed = true;
        }

        f = FrontierExtractor::extract(board);
    }
    return f;
}

struct ErrorStats {
    std::vector<double> absErrors; 
    long indeterminateCells = 0;   

    void add(double mcProb, double bfProb) {
        if (mcProb < 0.0) { 
            indeterminateCells++;
            return;
        }
        absErrors.push_back(std::abs(mcProb - bfProb));
    }

    double mean() const {
        if (absErrors.empty()) return 0.0;
        return std::accumulate(absErrors.begin(), absErrors.end(), 0.0) / absErrors.size();
    }

    double maxError() const {
        if (absErrors.empty()) return 0.0;
        return *std::max_element(absErrors.begin(), absErrors.end());
    }

    double stddev() const {
        if (absErrors.size() < 2) return 0.0;
        double m = mean();
        double sumSq = 0.0;
        for (double e : absErrors) sumSq += (e - m) * (e - m);
        return std::sqrt(sumSq / (absErrors.size() - 1));
    }
};

int main() {
    std::cout << "Benchmark determinístico linear (Sem Threads) iniciando...\n";

    const int width = 50;
    const int height = 50;
    const int mineCount = 380;      
    const int safeX = 22;
    const int safeY = 22;
    const int numTrials = 100;      // Configurado para 10 tabuleiros
    const int mcSamples = 1000;

    int trialsUsed = 0;
    int trialsSkippedEmptyFrontier = 0;
    int mcTimeoutCount = 0;         
    int bfTimeoutCount = 0;         

    ErrorStats globalStats;
    std::vector<long long> mcTimesMs;
    std::vector<long long> bfTimesMs;
    std::vector<size_t> frontierSizesUsed;
    std::vector<long> abortedAttemptsPerTrial; 

    for (int trial = 0; trial < numTrials; trial++) {
        Board board = BoardGenerator::generate(width, height, mineCount, safeX, safeY);
        revealCascade(board, safeX, safeY);

        Frontier f = stabilizeWithDeterministicSolver(board);

        if (f.frontierCells.empty()) {
            trialsSkippedEmptyFrontier++;
            continue;
        }

        frontierSizesUsed.push_back(f.frontierCells.size());
        trialsUsed++;

        // --- Execução do Monte Carlo ---
        MonteCarloSolver mc;
        auto mcStart = Clock::now();
        auto probsMC = mc.estimate(board, f, mcSamples);
        auto mcEnd = Clock::now();
        long long mcMs = std::chrono::duration_cast<std::chrono::milliseconds>(mcEnd - mcStart).count();
        
        bool mcTimedOut = mc.wasInterruptedByTimeout();
        if (mcTimedOut) mcTimeoutCount++;

        // --- Execução do Brute Force ---
        BruteForceSolver bf;
        auto bfStart = Clock::now();
        auto probsBF = bf.solve(board, f);
        auto bfEnd = Clock::now();
        long long bfMs = std::chrono::duration_cast<std::chrono::milliseconds>(bfEnd - bfStart).count();

        bool bfTimedOut = bf.wasInterruptedByTimeout();
        if (bfTimedOut) bfTimeoutCount++;

        // --- Acumula estatísticas apenas se ambos completaram sem estourar o tempo ---
        if (!mcTimedOut && !bfTimedOut) {
            for (const auto& cell : f.frontierCells) {
                double mcProb = probsMC.count(cell) ? probsMC.at(cell) : -1.0;
                double bfProb = probsBF.count(cell) ? probsBF.at(cell) : 0.0;
                globalStats.add(mcProb, bfProb);
            }
            mcTimesMs.push_back(mcMs);
            bfTimesMs.push_back(bfMs);
            abortedAttemptsPerTrial.push_back(mc.getAbortedAttempts());
        }

        // Saída em tempo real por trial
        std::cout << "  Trial " << std::setw(3) << trial << ": fronteira=" << std::setw(2) << f.frontierCells.size();
        if (mcTimedOut) std::cout << " | MC=TIMEOUT (>2min)";
        else std::cout << " | MC=" << mcMs << "ms";

        if (bfTimedOut) std::cout << " | BF=TIMEOUT (>2min)";
        else std::cout << " | BF=" << bfMs << "ms";
        
        std::cout << " | abortedAttempts=" << (mcTimedOut ? 0 : mc.getAbortedAttempts()) << "\n";
    }

    // ---------------------------------------------------------------
    // Relatório Final Consolidado
    // ---------------------------------------------------------------
    std::cout << "\n=== Resumo do Benchmark ===\n";
    std::cout << "Trials configurados: " << numTrials << "\n";
    std::cout << "Trials processados (com fronteira ativa): " << trialsUsed << "\n";
    std::cout << "Trials pulados (resolvidos pelo deterministico): " << trialsSkippedEmptyFrontier << "\n";

    std::cout << "\n--- Interrupções por Excesso de Tempo (Timeout > 2 min) ---\n";
    std::cout << "MonteCarlo interrompidos: " << mcTimeoutCount << "\n";
    std::cout << "BruteForce interrompidos: " << bfTimeoutCount << "\n";

    if (mcTimesMs.empty() || bfTimesMs.empty()) {
        std::cout << "\nNenhum trial foi concluído com sucesso por ambos os solucionadores dentro do tempo limite.\n";
        return 0;
    }

    double avgFrontier = std::accumulate(frontierSizesUsed.begin(), frontierSizesUsed.end(), 0.0) / frontierSizesUsed.size();
    double avgMcTime = std::accumulate(mcTimesMs.begin(), mcTimesMs.end(), 0.0) / mcTimesMs.size();
    double avgBfTime = std::accumulate(bfTimesMs.begin(), bfTimesMs.end(), 0.0) / bfTimesMs.size();
    double avgAborted = std::accumulate(abortedAttemptsPerTrial.begin(), abortedAttemptsPerTrial.end(), 0.0) / abortedAttemptsPerTrial.size();
    long totalAborted = std::accumulate(abortedAttemptsPerTrial.begin(), abortedAttemptsPerTrial.end(), 0L);

    std::cout << std::fixed << std::setprecision(4);
    std::cout << "\nTamanho medio da fronteira testada: " << avgFrontier << " celulas\n";
    std::cout << "Tempo medio MonteCarlo (concluidos): " << avgMcTime << " ms\n";
    std::cout << "Tempo medio BruteForce (concluidos): " << avgBfTime << " ms\n";

    std::cout << "\n--- Tentativas abortadas pelo orcamento (nodeLimit_) do MonteCarlo ---\n";
    std::cout << "Total de tentativas abortadas (soma de todos trials limpos): " << totalAborted << "\n";
    std::cout << "Media de tentativas abortadas por trial: " << avgAborted << "\n";

    std::cout << "\n--- Precisao do MonteCarlo (comparado ao BruteForce como gabarito) ---\n";
    std::cout << "Celulas comparadas com sucesso: " << globalStats.absErrors.size() << "\n";
    std::cout << "Celulas indeterminadas (MC nao conseguiu estimar): " << globalStats.indeterminateCells << "\n";
    std::cout << "Erro absoluto medio (MAE): " << (globalStats.mean() * 100.0) << " pontos percentuais\n";
    std::cout << "Erro absoluto maximo: " << (globalStats.maxError() * 100.0) << " pontos percentuais\n";
    std::cout << "Desvio padrao do erro: " << (globalStats.stddev() * 100.0) << " pontos percentuais\n";

    return 0;
}