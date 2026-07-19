#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>
#include <chrono>
#include <numeric>
#include <algorithm>
#include "Board.h"
#include "BoardGenerator.h"
#include "FrontierExtractor.h"
#include "DeterministicSolver.h"
#include "BruteForceSolver.h"
#include "AutoPlayer.h"
#include "MonteCarloSolver.h"

using namespace ms;
using Clock = std::chrono::high_resolution_clock;

// --- funcoes auxiliares, identicas as ja usadas nos outros mains ---
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

// Guarda o resultado bruto de UM trial -- assim rodamos tudo primeiro,
// e so depois decidimos como formatar/imprimir, sem misturar as duas coisas.
struct TrialResult {
    int trialIndex;
    size_t frontierSize;
    bool mcTimedOut;
    bool bfTimedOut;
    long long mcMs;
    long long bfMs;
    long abortedNodes;
};

void printTrialTable(const std::vector<TrialResult>& trials) {
    std::cout << "\n--- Trials individuais ---\n";
    std::cout << std::left
               << std::setw(8)  << "Trial"
               << std::setw(12) << "Fronteira"
               << std::setw(16) << "MonteCarlo"
               << std::setw(16) << "BruteForce"
               << std::setw(14) << "NosPodados"
               << "\n";
    std::cout << std::string(66, '-') << "\n";

    for (const auto& t : trials) {
        std::cout << std::left
                   << std::setw(8)  << t.trialIndex
                   << std::setw(12) << t.frontierSize
                   << std::setw(16) << (t.mcTimedOut ? std::string("TIMEOUT(>2min)") : (std::to_string(t.mcMs) + "ms"))
                   << std::setw(16) << (t.bfTimedOut ? std::string("TIMEOUT(>2min)") : (std::to_string(t.bfMs) + "ms"))
                   << std::setw(14) << (t.mcTimedOut ? std::string("-") : std::to_string(t.abortedNodes))
                   << "\n";
    }
}

void printSummary(int numTrials, int trialsUsed, int trialsSkippedEmptyFrontier,
                   int mcTimeoutCount, int bfTimeoutCount,
                   const std::vector<size_t>& frontierSizesUsed,
                   const std::vector<long long>& mcTimesMs,
                   const std::vector<long long>& bfTimesMs,
                   const std::vector<long>& abortedAttemptsPerTrial,
                   const ErrorStats& globalStats) {

    std::cout << "\n=== Resumo do Benchmark ===\n";
    std::cout << std::left
               << std::setw(45) << "Trials configurados"                        << numTrials << "\n"
               << std::setw(45) << "Trials processados (fronteira ativa)"       << trialsUsed << "\n"
               << std::setw(45) << "Trials pulados (resolvidos pelo deterministico)" << trialsSkippedEmptyFrontier << "\n";

    std::cout << "\n--- Timeouts (> 2 min) ---\n";
    std::cout << std::left
               << std::setw(45) << "MonteCarlo interrompidos"  << mcTimeoutCount << "\n"
               << std::setw(45) << "BruteForce interrompidos"  << bfTimeoutCount << "\n";

    if (mcTimesMs.empty() || bfTimesMs.empty()) {
        std::cout << "\nNenhum trial concluido com sucesso por ambos os solucionadores dentro do tempo limite.\n";
        return;
    }

    double avgFrontier = std::accumulate(frontierSizesUsed.begin(), frontierSizesUsed.end(), 0.0) / frontierSizesUsed.size();
    double avgMcTime = std::accumulate(mcTimesMs.begin(), mcTimesMs.end(), 0.0) / mcTimesMs.size();
    double avgBfTime = std::accumulate(bfTimesMs.begin(), bfTimesMs.end(), 0.0) / bfTimesMs.size();
    double avgAborted = std::accumulate(abortedAttemptsPerTrial.begin(), abortedAttemptsPerTrial.end(), 0.0) / abortedAttemptsPerTrial.size();
    long totalAborted = std::accumulate(abortedAttemptsPerTrial.begin(), abortedAttemptsPerTrial.end(), 0L);

    std::cout << std::fixed << std::setprecision(2);

    std::cout << "\n--- Tempo e escala (apenas trials concluidos, sem timeout) ---\n";
    std::cout << std::left
               << std::setw(45) << "Tamanho medio da fronteira"     << avgFrontier << " celulas\n"
               << std::setw(45) << "Tempo medio MonteCarlo"         << avgMcTime   << " ms\n"
               << std::setw(45) << "Tempo medio BruteForce"         << avgBfTime   << " ms\n";

    std::cout << "\n--- Nos podados durante a travessia unica (MonteCarlo) ---\n";
    std::cout << std::left
               << std::setw(45) << "Total (soma de todos trials limpos)" << totalAborted << "\n"
               << std::setw(45) << "Media por trial"                     << avgAborted << "\n";

    std::cout << std::setprecision(4);
    std::cout << "\n--- Precisao do MonteCarlo (BruteForce como gabarito) ---\n";
    std::cout << std::left
               << std::setw(45) << "Celulas comparadas com sucesso"        << globalStats.absErrors.size() << "\n"
               << std::setw(45) << "Celulas indeterminadas (MC nao estimou)" << globalStats.indeterminateCells << "\n"
               << std::setw(45) << "Erro absoluto medio (MAE)"             << (globalStats.mean() * 100.0)     << " pontos percentuais\n"
               << std::setw(45) << "Erro absoluto maximo"                  << (globalStats.maxError() * 100.0) << " pontos percentuais\n"
               << std::setw(45) << "Desvio padrao do erro"                 << (globalStats.stddev() * 100.0)   << " pontos percentuais\n";
}

int main() {
    std::cout << "=== Benchmark: MonteCarloSolver (travessia unica) vs BruteForce ===\n";

    const int width = 32;
    const int height = 32;
    const int mineCount = 360;
    const int safeX = 22;
    const int safeY = 22;
    const int numTrials = 100;
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
    std::vector<TrialResult> trialResults; // ADIÇÃO: guarda cada trial pra imprimir depois, formatado

    std::cout << "\nRodando " << numTrials << " trials, board " << width << "x" << height
               << " (" << mineCount << " minas)...\n";

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

        // --- Monte Carlo (travessia unica, sem reset) ---
        MonteCarloSolver mc;
        auto mcStart = Clock::now();
        auto probsMC = mc.estimate(board, f, mcSamples);
        auto mcEnd = Clock::now();
        long long mcMs = std::chrono::duration_cast<std::chrono::milliseconds>(mcEnd - mcStart).count();
        bool mcTimedOut = mc.wasInterruptedByTimeout();
        if (mcTimedOut) mcTimeoutCount++;

        // --- Brute Force (gabarito) ---
        BruteForceSolver bf;
        auto bfStart = Clock::now();
        auto probsBF = bf.solve(board, f);
        auto bfEnd = Clock::now();
        long long bfMs = std::chrono::duration_cast<std::chrono::milliseconds>(bfEnd - bfStart).count();
        bool bfTimedOut = bf.wasInterruptedByTimeout();
        if (bfTimedOut) bfTimeoutCount++;

        long abortedNodes = mcTimedOut ? 0 : mc.getAbortedAttempts();

        if (!mcTimedOut && !bfTimedOut) {
            for (const auto& cell : f.frontierCells) {
                double mcProb = probsMC.count(cell) ? probsMC.at(cell) : -1.0;
                double bfProb = probsBF.count(cell) ? probsBF.at(cell) : 0.0;
                globalStats.add(mcProb, bfProb);
            }
            mcTimesMs.push_back(mcMs);
            bfTimesMs.push_back(bfMs);
            abortedAttemptsPerTrial.push_back(abortedNodes);
        }

        trialResults.push_back({trial, f.frontierCells.size(), mcTimedOut, bfTimedOut, mcMs, bfMs, abortedNodes});
    }

    std::cout << "Concluido.\n";

    // --- Impressao em blocos separados, sem misturar dado individual com resumo ---
    printTrialTable(trialResults);
    printSummary(numTrials, trialsUsed, trialsSkippedEmptyFrontier,
                 mcTimeoutCount, bfTimeoutCount,
                 frontierSizesUsed, mcTimesMs, bfTimesMs,
                 abortedAttemptsPerTrial, globalStats);

    return 0;
}
