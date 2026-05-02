#include <iostream>
#include <vector>
#include <fstream>
#include "../buildgraph/buildgraph.h"
#include <chrono>

using namespace std; //importa o lugar onde ficam as funções std ai n precisa ficar usando std::

typedef struct {
    int cost;
    vector<int> path;
} bestPath;

void updateCost(vector<int>& v, bestPath& opt, vector<vector<int>>& graph) {
    int cost = 0;
    int origin_idx = v[0];
    int i;
    int atual;
    int prox;

    // calculates the cost of thy path
    for (i = 0; i < (v.size() - 1); i++) {
        atual = v[i];
        prox = v[i+1];

        //aresta do vertice atual até o próx
        cost += graph[atual][prox]; // como acessar o próximo elemento do vetor?
    }

    // cout << prox << "\n";
    //a última aresta volta pro ínicio
    cost += graph[prox][origin_idx]; //essa parte aqui tá dando errada

    // update best cost known
    if(cost < opt.cost) {
        opt.cost = cost;
        opt.path = v; //isso existe em cpp?
    }
}

/*
@brief recebe um vetor por referência, ou seja, vai trabalhar sempre em cima do mesmo vetor
utiliza dois ponteiros, l e r para percorrer o vetor.
l é o nível da recursão, também indica que todos números antes deles já estão fixos naquela variação da permutação
r é o ponteiro que indica o fim do vetor, se l chegou em r terminamos de gerar uma das permutações de vértices
i marca a posição do swap.

não é nem um pouco intuitivo. Tem que desenhar a stack e executar passo a passo no papel.
Não é intuitivo pq ele não registra as permutações em ordem. É como se ele tivesse percorrendo uma árvore, mas sem criar ela.
Depois, ao final da permutação ele desfaz o último swap e testa todas. Como se fosse voltar pra cima na árvore.
*/
void permute(vector<int>& v, int l, int r, bestPath& opt, vector<vector<int>>& graph) {

    if (l == r) { // valid permutation

        // retira simetria reversa (não calculca caminhos iguais)
        // [ 0 2 1 ] = [ 1 2 0 ], usa uma regra arbitrária e evita calculcar esses 2
        // em grafos menors como o tsp2 ele impacta até negativamente (aumento de ~40µ)
        // em grafos maiores como o tsp1 ele impacta muito positivamente (diminui tempo de exec em ~2600ms)
        if (v[1] < v[(v.size()-1)]) {
            updateCost(v, opt, graph);    
        }
        
        // caso queira executar sem retirar os casos de simetria:
        // updateCost(v, opt, graph);

        return;
    }

    for (int i = l; i < r; i++) {
        swap(v[l], v[i]);
        permute(v, l + 1, r, opt, graph);
        swap(v[l], v[i]); // volta (backtrack)
    }
}

int main() {
    fstream tsp ("data/tsp1_253.txt", fstream::in);
    vector<vector<int>> graph = buildGraph(tsp);    // 1n
    vector<int> v; //vertices

    for(int i = 0; i < graph.size(); i++) {
        v.push_back(i);
    }

    bestPath opt;
    opt.cost = __INT_MAX__;

    // auto init = chrono::high_resolution_clock::now();
    permute(v, 0, v.size(), opt, graph);
    // auto res = chrono::high_resolution_clock::now() - init;

    // long long milliseconds = chrono::duration_cast<chrono::milliseconds>(res).count();
    // cout << "\nretirando simetria reversa: " << milliseconds << "µs\n\n";
    // cout << "\nsem tratar simetria reversa: " << milliseconds << "ms\n\n";

    cout << opt.cost << "\nPath: ";
    for(int x : opt.path) cout << x << " ";
    cout << "\n";
}