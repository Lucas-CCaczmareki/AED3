#include "MonteCarloSolver.h"

namespace ms {

std::unordered_map<std::pair<int,int>, double, PairHash>
MonteCarloSolver::estimate(const Board& board, const Frontier& frontier, int numSamples) {
    
    // Isso retorna uma porcentagem de chance de bomba pra cada variável (célula enscondida da fronteira)
    std::unordered_map<std::pair<int,int>, double, PairHash> result;

    
    // TODO: gerar numSamples disposicoes (idealmente via backtracking com poda),
    // filtrar validas, contar frequencia de bomba por celula, dividir pelo total valido.

    // descarta os valores aqui só pra conseguir compilar certinho, dps tira esses (void)
    (void)board;
    (void)frontier;
    (void)numSamples;










    return result;
}

/*
pra cada frontierNumber, vai pegar todas células não reveladas em volta (variáveis)
e vai criar uma estrutura Constraint e jogar ela no contraints_
*/
void MonteCarloSolver::buildConstraints(const Board& board, const Frontier& frontier) {

    //a fronteira já vem definida
    for(std::pair<int, int> p : frontier.frontierNumbers){
        int x = p.first, y = p.second;
        Cell c = board.at(x, y);

        Constraint constraint;
        int flaggedCount = 0; //conta as q sao flagged pra tirar do adjacentMines depoi

        // pra todas células adjacentes
        for(int adjY=(y-1); adjY <= (y+1); adjY++) {
            for(int adjX=(x-1); adjX <= (x+1); adjX++) {
                if(adjX == x && adjY == y) { continue; } //ignora a celula central

                if(board.inBounds(adjX, adjY)) {

                    //adiciona só as casas q tão 100% no escuro
                    if (board.at(adjX, adjY).state == CellState::Covered) {
                        constraint.variables.push_back(std::pair(adjX, adjY));
                    
                    //conta quantas casas adjacentes flagged tem
                    } else if (board.at(adjX, adjY).state == CellState::Flagged) {
                        flaggedCount++;
                    }
                }
            }
        }
        constraint.target = c.adjacentMines - flaggedCount;
        constraints_.push_back(constraint);
    }
}

/*
Pra todas variáveis (celulas escondidas) da fronteira (frontierCells)
vai contar a incidência e ordenar elas num vetor
*/
void MonteCarloSolver::sortVariables(const Frontier& frontier) {

    std::unordered_map<std::pair<int, int>, int, PairHash> appearanceCount; //tem como inicializar com 0?

    // Varre direto as restrições já criadas e conta a incidência
    for (const Constraint& constr : constraints_) {
        for (const std::pair<int, int>& cell : constr.variables) {
            // Se cell não existir no mapa, o C++ cria valendo 0 e soma 1.
            // Se já existir, ele só soma 1. Acontece por causa do operador[]
            appearanceCount[cell]++; 
        }
    }

    // Copia as variáveis da fronteira para o vetor ordenado da classe
    orderedVariables_ = frontier.frontierCells;

    // Aplica o std::sort com uma função de comparação específica
    std::sort(orderedVariables_.begin(), orderedVariables_.end(), 
        
        // uso da função lambda aqui pra facilitar a vida e não fugir do escopo de sortVariables
        // passando appearanceCount por ponteiro pra uma função externa.

        //função lambda de comparação ">"" entre a e b utilizando o appearanceCount:
        [&appearanceCount](const std::pair<int, int>& a, const std::pair<int, int>& b) {
            
            // Se 'a' ou 'b' não aparecerem em nenhuma restrição (.count retorna null)
            // assume o valor como 0. Caso apareceu, usa o .at() pra ler o valor
            int countA = appearanceCount.count(a) != 0 ? appearanceCount.at(a) : 0;
            int countB = appearanceCount.count(b) != 0 ? appearanceCount.at(b) : 0;

            // o retorno true coloca A antes de B, false faz o contrário.
            return countA > countB; // Maior incidência primeiro
        }
    );
}


/*

*/
void MonteCarloSolver::backtrack (size_t index, std::unordered_map<std::pair<int, int>, int, PairHash>& currAssignments) {

    //condições de retorno no backtrack
    if (validSolutions_.size() == numSamples_) { return; }
    
    if (index == orderedVariables_.size()) { 
        validSolutions_.push_back(currAssignments);
        return; 
    }    

    //pega a primeira variavel
    std::pair<int, int> currentVar = orderedVariables_[index];

    //cria conjunto e aleatoriza a ordem pra testar as vezes o 0 primeiro, as vezes o 1
    std::vector<int> valueOrder = {0, 1};
    std::shuffle(valueOrder.begin(), valueOrder.end(), rng_);

    // 0 = notBomb, 1 = bomb
    for(int value : valueOrder) {
        //assume um valor, ora 0, ora 1 (aleatório)
        currAssignments[currentVar] = value;

        if(violatesConstraints(currentVar, currAssignments)) {
            currAssignments.erase(currentVar); //desfaz
            continue; //testa o próximo valor
        }

        backtrack(index + 1, currAssignments); //continua descendo até o ponto + profundo do ramo
        currAssignments.erase(currentVar); //permite retorno e testar o próx valor se essa era válida

        // para de descer na árvore antes de testar todos valores pra essa variável  
        if (validSolutions_.size() >= (size_t)numSamples_) {
            return;
        }

    }
}

bool MonteCarloSolver::violatesConstraints(const std::pair<int, int>& lastAssigned, const std::unordered_map<std::pair<int, int>, int, PairHash>& currAssignments) {
    // for(Constraint c : constraints_) {
    //     for(std::pair<int, int> p : c.variables) {
    //         if(p.first == lastAssigned.first && p.second == lastAssigned.second) {

    //         }
    //     }
    // }
    // nao consigo mais pensar, amanha eu volto a mexer nisso
}


} // namespace ms
