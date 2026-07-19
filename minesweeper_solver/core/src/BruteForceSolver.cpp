#include "BruteForceSolver.h"

namespace ms {

// Tudo aqui é basicamente copia e cola do monte carlo
// eu só tirei os limites de geração de samples

std::unordered_map<std::pair<int,int>, double, PairHash>
BruteForceSolver::solve(const Board& board, const Frontier& frontier) {
    
    timeoutOccurred_ = false;
    auto endTime = std::chrono::high_resolution_clock::now() + std::chrono::minutes(1);

    // reseta o estado da instância antes de cada uso
    constraints_.clear();
    orderedVariables_.clear();
    validSolutions_.clear();
    variableConstraints_.clear();

    // Isso retorna uma porcentagem de chance de bomba pra cada variável (célula enscondida da fronteira)
    std::unordered_map<std::pair<int,int>, double, PairHash> result;
    std::unordered_map<std::pair<int, int>, int, PairHash> currAssignments;  // vazio

    // TODO: enumerar todas as 2^n combinacoes da fronteira, filtrar validas,
    // contar frequencia exata de bomba por celula.

    buildConstraints(board, frontier);
    sortVariables(frontier);
    
    backtrack(0, currAssignments, endTime);

    for(const std::pair<int, int>& cell : frontier.frontierCells) { //p/ cada célula na fronteira
        int appearAsBomb = 0;
        
        for(const auto& sample : validSolutions_) {
            if(sample.at(cell) == 1) { //se apareceu nessa disposição como bomba
                appearAsBomb++;
            }
        }

        //usa validSolutions.size ao invés de numSamples pq numSamples é um limite, pode ser
        // que seja impossível gerar tantas numSamples (ex: tab 4x4, n da pra gerar 1000 samples diferentes)
        result[cell] = (double)appearAsBomb / (validSolutions_.size());
    }

    return result;
}

void BruteForceSolver::buildConstraints(const Board& board, const Frontier& frontier) {

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

                        //variableConstraints_[std::pair(adjX, adjY)]:
                            // cria no map a entrada desse pair se já n tiver e acessa o vetor
                        //.push_back(constraints_.size());
                            //pega o vetor daquela posição e coloca o último índice de constraints_
                        variableConstraints_[std::pair(adjX, adjY)].push_back(constraints_.size());
                    
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
void BruteForceSolver::sortVariables(const Frontier& frontier) {

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
explicar isso direitinho dps
*/
void BruteForceSolver::backtrack (size_t index, std::unordered_map<std::pair<int, int>, int, PairHash>& currAssignments, std::chrono::high_resolution_clock::time_point endTime) {
    
    if (std::chrono::high_resolution_clock::now() > endTime) {
        timeoutOccurred_ = true;
        return;
    }

    if (index == orderedVariables_.size()) { 
        validSolutions_.push_back(currAssignments);
        return; 
    }    

    //pega a primeira variavel
    std::pair<int, int> currentVar = orderedVariables_[index];
    std::vector<int> valueOrder = {0, 1};

    // 0 = notBomb, 1 = bomb
    for(int value : valueOrder) {
        //assume um valor, ora 0, ora 1 (aleatório)
        currAssignments[currentVar] = value;

        if(violatesConstraints(currentVar, currAssignments)) {
            currAssignments.erase(currentVar); //desfaz
            continue; //testa o próximo valor
        }

        backtrack(index + 1, currAssignments, endTime); //continua descendo até o ponto + profundo do ramo
        currAssignments.erase(currentVar); //permite retorno e testar o próx valor se essa era válida
    }
}

/*
    // Tem 2 jeitos de violar a restrição já just in time
    // 1o: A soma das variáveis nessa disposição do tabuleiro (parcial) já superou o target (numero)
    // 2o: Todas variaveis da restrição já tem um valor atribuído e a soma não deu igual ao target

    // infos disponíveis nesse escopo:
    // lastAssigned: variavel q teve valor atribuida. ex: (0, 1)
    // currAssigments: todos valores atribuidos pras variaveis até então nesse ramo
    // constraints_: um vetor, com todas restrições pra cada número da fronteira
    // variableConstraints_ : map pra (x,y) -> vetor com indices das restrições que tem aquela variavel
*/
bool BruteForceSolver::violatesConstraints(const std::pair<int, int>& lastAssigned, const std::unordered_map<std::pair<int, int>, int, PairHash>& currAssignments) {
    
    
    // como percorrer todas constraints e todas variaveis de cada constraint é péssimo
    // eu optei por criar um map (x,y) -> vetor com indices das constraints onde ela aparece
    // o overhead de operação é menor (roda menos vezes) e não ocupa tanta memória com size_t
    
    for(int i = 0; i < variableConstraints_[lastAssigned].size(); i++) {

        int sum = 0;
        bool allVarAssigned = true;
        //vai percorrer as variaveis da constraint e ver quanto ela ta somando
        for(const std::pair<int, int>& var : constraints_[variableConstraints_[lastAssigned][i]].variables) { //acessa a constraint q tem a ultima variavel
            if(currAssignments.count(var) > 0) { //se a variavel já teve valor atribuido
                sum += currAssignments.at(var);

            //se n teve valor atribuido n soma. Porem seta flag como false
            } else  {
                allVarAssigned = false;
            }
        }

        //1a forma de violar constraint
        if (sum > constraints_[variableConstraints_[lastAssigned][i]].target) {
            return true; //violou constraint
        }

        //2a forma de violar constraint
        if(allVarAssigned) {
            if(sum != constraints_[variableConstraints_[lastAssigned][i]].target) {
                return true;
            }
        }
    }
    //se n teve nenhuma das 2 violações q eu previ pra todas restrições q a lastAssigned tá, retorna false
    return false; 
}

} // namespace ms

