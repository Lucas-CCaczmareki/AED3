#include "MonteCarloSolver.h"

namespace ms {

std::unordered_map<std::pair<int,int>, double, PairHash>
MonteCarloSolver::estimate(const Board& board, const Frontier& frontier, int numSamples) {
    
    timeoutOccurred_ = false;
    auto endTime = std::chrono::high_resolution_clock::now() + std::chrono::minutes(2);

    // reseta o estado da instância antes de cada uso
    constraints_.clear();
    orderedVariables_.clear();
    validSolutions_.clear();
    variableConstraints_.clear();
    rng_.seed(std::random_device{}());
    nodesVisited_ = 0;
    abortedAttempts_ = 0;

    // Isso retorna uma porcentagem de chance de bomba pra cada variável (célula enscondida da fronteira)
    std::unordered_map<std::pair<int,int>, double, PairHash> result;
    std::unordered_map<std::pair<int, int>, int, PairHash> currAssignments;  // vazio
    numSamples_ = numSamples;

    buildConstraints(board, frontier);
    sortVariables(frontier);
    
    // CORREÇÃO: antes tinha um while() aqui que reiniciava currAssignments.clear() e chamava
    // backtrack(0, ...) de novo do ZERO toda vez que uma amostra era achada (ou uma tentativa
    // era abortada). Isso fazia a árvore inteira ser re-percorrida do início pra cada amostra
    // individual. Em fronteiras grandes, isso é ordens de magnitude mais lento que uma
    // travessia única (o mesmo motivo pelo qual o BruteForceSolver, que faz só UMA chamada de
    // backtrack, ficava mais rápido que o Monte Carlo em fronteiras de 70-100+ células, o que
    // não devia acontecer já que o Monte Carlo é a abordagem que deveria escalar melhor).
    //
    // Agora é UMA chamada só. A função backtrack cuida de continuar coletando amostras pelos
    // outros ramos da árvore em vez de parar no primeiro achado, e só propaga "parar" quando
    // bate numSamples_, estoura nodeLimit_ ou dá timeout.
    backtrack(0, currAssignments, endTime);

    //informações interessantes nesse escopo
    // frontier.frontierCells -> todas células escondidas da fronteira
    // validSolutions_ -> vetor de {tabela com todas celulas da fronteira mapeando pro valor atribuido}

    // se o orçamento estourou (ou deu timeout) e nao sobrou nenhuma solucao valida,
    // reporta como indeterminado (-1.0) em vez de dividir por zero.
    if (validSolutions_.empty()) {
        for (const std::pair<int, int>& cell : frontier.frontierCells) {
            result[cell] = -1.0;
        }
        return result;
    }

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
explicar isso direitinho dps

essa função antes tinha um "long& nodesVisited" que era uma variável LOCAL de cada
chamada de backtrack(0, ...) lá em estimate() (resetava a cada reinício da árvore). Agora
nodesVisited_ é atributo da classe (nodesVisited_), porque a travessia é uma só do início ao
fim
*/
bool MonteCarloSolver::backtrack (size_t index, std::unordered_map<std::pair<int, int>, int, PairHash>& currAssignments, std::chrono::high_resolution_clock::time_point endTime) {

    // Checagem de Estouro de Tempo (Injetado)
    if (std::chrono::high_resolution_clock::now() > endTime) {
        timeoutOccurred_ = true;
        return true; // propaga "para tudo" até a raiz
    }

    // orçamento de esforço GLOBAL pra travessia inteira. Se estourar, desiste de
    // continuar cavando. É isso que impede herdar o pior caso exponencial do brute force
    // sem precisar reiniciar a árvore do zero a cada amostra.
    if (++nodesVisited_ > nodeLimit_) {
        return true; // propaga "para tudo" até a raiz
    }

    //condições de retorno no backtrack
    if (index == orderedVariables_.size()) {
        validSolutions_.push_back(currAssignments);

        // CORREÇÃO: antes era "return true" direto (achou 1 solução = para tudo).
        // Agora só para de verdade quando já juntou numSamples_ amostras -- senão,
        // devolve false e deixa o laço de valueOrder lá embaixo continuar tentando
        // o outro valor da variável anterior, seguindo pelos outros ramos da árvore.
        return (long)validSolutions_.size() >= numSamples_;
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
            abortedAttempts_++; // ramo podado (beco sem saída descartado)
            continue; //testa o próximo valor
        }

        //continua descendo até o ponto + profundo do ramo
        if(backtrack(index + 1, currAssignments, endTime)) {
            currAssignments.erase(currentVar); //permite retorno e testar o próx valor se essa era válida
            return true; // meta batida (ou orçamento/timeout estourado) -- sobe sem testar o outro valor
        } 
        currAssignments.erase(currentVar);
    }
    return false; //esgotou os dois ramos aqui, sobe pro nível anterior tentar o outro valor dele
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
bool MonteCarloSolver::violatesConstraints(const std::pair<int, int>& lastAssigned, const std::unordered_map<std::pair<int, int>, int, PairHash>& currAssignments) {
    
    
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