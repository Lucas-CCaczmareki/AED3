#include "../include/Board.h"

namespace ms {

// construtor
Board::Board(int width, int height)
    : width_(width), height_(height), cells_(width * height) {} 

bool Board::inBounds(int x, int y) const {
    return x >= 0 && x < width_ && y >= 0 && y < height_;
}

Cell& Board::at(int x, int y) {
    return cells_[y * width_ + x];
}

const Cell& Board::at(int x, int y) const {
    return cells_[y * width_ + x];
}

std::vector<std::pair<int, int>> Board::neighbors(int x, int y) const {
    std::vector<std::pair<int, int>> results;
    // primeiro colunas x dps linhas y
    for(int j = -1; j <= 1; j++) {
        for(int i = -1; i <= 1; i++) {
            if (i == 0 && j == 0) continue;
            int jx = j + x;
            int iy = i + y;
            if (inBounds(jx, iy)) {
                results.push_back({jx, iy});
            }
        }
    }
    return results;
}


} // namespace ms
