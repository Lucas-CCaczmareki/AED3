#include "../include/BoardGenerator.h"

namespace ms {
    
// a safe zone vai ser as 8 celulas em volta do primeiro click
bool BoardGenerator::isSafeZone(int x, int y, int clickX, int clickY) {
    int jx =  std::abs(x - clickX); // com unsigned int dava problemas, ent tive q usar stad::abs
    int iy =  std::abs(y - clickY);

    return jx <= 1 && iy <= 1;
}

Board BoardGenerator::generate(int width, int height, int mineCount, int safeX, int safeY) {
    std::random_device rd;
    return generate(width, height, mineCount, safeX, safeY, rd());
}

Board BoardGenerator::generate(int width, int height, int mineCount, int safeX, int safeY, unsigned int seed) {
    if (mineCount > width * height) {
        throw std::runtime_error("Quantidade de bombas maior q tabuleiro");
    }

    Board board(width, height);

    // exclui a parte segura, em volta do primeiro clique, da disposiçao de bombas
    std::vector<std::pair<int, int>> candidates;
    for(int y = 0; y < height; y++ ) { // linha
        for(int x = 0; x < width; x++) { // coluna
            if(isSafeZone(x, y, safeX, safeY)) continue;
            candidates.push_back({x, y});
        }
    }
    
    // aleatoriza a geraçao de bombas
    std::mt19937 rng(seed); //usa a seed fixada ou aleatória
    std::shuffle(candidates.begin(), candidates.end(), rng); // shuffle reorganiza o vetor aleatóriamente, vou pegar a quantidade de bombas necessárias pelo começo do vetor
    for(int i = 0; i < mineCount; i++) {
        int x = candidates[i].first;
        int y = candidates[i].second;
        board.at(x, y).isMine = true;
    }
    
    // agr seta a cardinalidade?? de cada celula
    for(int y = 0; y < height; y++ ) { // linha
        for(int x = 0; x < width; x++) { // coluna
            if(board.at(x, y).isMine) continue;
            
            std::vector<std::pair<int, int>> neighbords = board.neighbors(x, y);

            int bombCaunt = 0;
            for(auto [nx, ny] : board.neighbors(x, y)) {
                if(board.at(nx, ny).isMine) bombCaunt++;
            }
            board.at(x, y).adjacentMines = bombCaunt;
        }
    }

    return board;
}

} // namespace ms

/*
CONSTATAÇOES:
    - a celula em que clica primeiro sempre vai ter um numero de bombas ao redor como 0, fui jogando várias vezes pra perceber
    - a probabilidade das casas ao redor da primeira serem 0 parece ser mais alta tbm 
*/ 