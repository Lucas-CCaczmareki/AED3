#pragma once
#include <vector>

// namespace evita que dê conflito com alguma outra classe Cell por exemplo (vaiq o qt tem uma ne)
// pra usar só faz um ms::Cell, ou declara que ta usando esse namespace
namespace ms {

// Cria um tipo novo com valores limitados
enum class CellState { Covered, Revealed, Flagged };

struct Cell {
    bool isMine = false;
    int adjacentMines = 0;
    CellState state = CellState::Covered;
};

class Board {
public:
    //Construtor
    Board(int width, int height);

    // Métodos get
    int width() const { return width_; }
    int height() const { return height_; }
    
    Cell& at(int x, int y);             //permite modificar
    const Cell& at(int x, int y) const; //bloqueia modificação

    // Diz se tá nos limites do tabuleiro
    bool inBounds(int x, int y) const;

private:
    // usa uma convenção "_" para diferenciar atributos privados de atributos locais com mesmo nome.
    int width_;
    int height_;
    std::vector<Cell> cells_;
};

} // namespace ms
