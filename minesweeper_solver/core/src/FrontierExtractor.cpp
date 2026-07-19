#include "FrontierExtractor.h"
#include <unordered_set>

namespace ms {

Frontier FrontierExtractor::extract(const Board& board) {
    Frontier f;

    // Usa um unordered set pra conseguir inserir e tratar coordenadas
    // repetidas (checagem) com custo ~O(1). Usa a hash pra achar rápido e ver se já tem um igual
    // como set vem dos conjuntos da matemática, isso bloqueia a inserção de um repetido.
    std::unordered_set<std::pair<int, int>, PairHash> frontierCellsSet;
    
    
    // TODO: percorrer board, achar numeros com vizinha coberta e as tais vizinhas
    board;

    for(int y = 0; y < board.height(); y++) { //row
        for(int x = 0; x < board.width(); x++) { //col
            //células (0,0), (0,1), (0,2)... no estilo (colX, rowY)
            //pra cada célula, vou analisar os vizinhos.

            Cell c = board.at(x, y);
            
            //ignora as reveladas com 0.
            if (c.state == CellState::Revealed && c.adjacentMines > 0) {
                f.frontierNumbers.push_back(std::pair(x, y));
            }

            //esse if tem 9 iterações no máximo
            if (c.state == CellState::Revealed) { //apenas uma célula coberta é fronteira
                //analisa as 8 adjacentes
                
                //bound é x - 1 até x + 1
                // e y - 1 até y + 1. Pras 8 células adjacentes
                for(int adjY=(y-1); adjY <= (y+1); adjY++) {
                    for(int adjX=(x-1); adjX <= (x+1); adjX++) {
                        
                        if(adjX == x && adjY == y) { continue; } //ignora a celula central

                        // se ta dentro do tabuleiro, avalia
                        if(board.inBounds(adjX, adjY)) {
                            if (board.at(adjX, adjY).state == CellState::Covered) {
                                //manda a célula atual pro vetor de células fronteiras
                                frontierCellsSet.insert(std::pair(adjX, adjY)); 
                            }
                        }
                    }
                }
            }

        }
    }
    // copia o conteúdo do set pra dentro do vector
    // paga o custo de O(n) só aqui pra copiar e manter expondo vector, que é mais rápido e simples de lidar que o unordered map (hashtable)
    f.frontierCells.assign(frontierCellsSet.begin(), frontierCellsSet.end());
    return f;
}

} // namespace ms
