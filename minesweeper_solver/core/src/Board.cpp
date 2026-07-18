#include "Board.h"

namespace ms {

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

} // namespace ms
