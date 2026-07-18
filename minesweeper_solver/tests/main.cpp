#include <iostream>
#include "Board.h"

int main() {
    ms::Board board(8, 8);
    if (board.width() == 8 && board.height() == 8) {
        std::cout << "OK: Board criado com dimensoes corretas.\n";
        return 0;
    }
    std::cerr << "FALHOU: dimensoes incorretas.\n";
    return 1;
}
