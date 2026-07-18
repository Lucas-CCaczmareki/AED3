#pragma once
#include <vector>

// namespace evita que dê conflito com alguma outra classe Cell por exemplo (vaiq o qt tem uma ne)
// pra usar só faz um ms::Cell, ou declara que ta usando esse namespace
namespace ms {

// Cria um tipo novo com valores limitados
enum class CellState { Covered, Revealed, Flagged };

struct Cell {
    bool isMine = false;                    //a bandenderinha que marca a célula
    int adjacentMines = 0;                  //numero que a casinha mostra
    CellState state = CellState::Covered;   //estado dela, pode ser econdida, revelada (aparece numero) ou marcada
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

    // acesso com (coluna, linha) = (x, y)
    // isso acontece pq em comp gráfica, coluna é x e linha é o y.
    std::vector<Cell> cells_; 
};

} // namespace ms
