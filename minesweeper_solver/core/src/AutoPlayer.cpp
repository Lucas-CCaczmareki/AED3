#include "AutoPlayer.h"
#include "DeterministicSolver.h"
#include "FrontierExtractor.h"
#include "MonteCarloSolver.h"

namespace ms {

GameResult AutoPlayer::playGame(Board& board, int monteCarloSamples) {

    // TODO: loop -> DeterministicSolver -> se travar -> FrontierExtractor + MonteCarloSolver
    // -> escolhe celula com menor probabilidade -> revela -> repete ate ganhar/perder.

    // descarta os valores aqui só pra conseguir compilar certinho, dps tira esses (void)
    (void)board;
    (void)monteCarloSamples;
    return GameResult::Ongoing;
}

} // namespace ms
