#include "dfs.h"
#include "buildgraph/buildgraph.h"
#include "prim_mst/prim_mst.h"
#include <fstream>
#include <stack>

/*
ATENÇÃO!!!!
TREE NO PARÂMETRO É REPRESENTADA ATRAVÉS DE UMA LISTA DE ADJACÊCIA
*/
void dfs(vector<vector<int>> tree, vector<int>& path) {
    //vou escolher arbitrariamente a origem como 0. Mas poderia ser randômica 
    int origin_idx = 5; // usando 5 pq o exemplo funcionou melhor

    // cria uma estrutura com visiteds
    vector<bool> visited; 

    //procurar um jeito melhor de inicializar depois
    for(int i = 0; i < tree.size(); i++) {
        visited[i] = 0;
    }
    visited[origin_idx] = 1;

    // cria uma stack ou usa recursão? em memória tanto faz, vai ter que usar uma mesmo, vou preferir explícita q é mais fácil de pensar
    stack<int> stk;
    stk.push(origin_idx);

    // loopa enquanto a stack não estiver vazia?
    while(!stk.empty()) {
        // desce sempre pelo primeiro da lista de adjacência
        // atualiza o path com os pushes na stack (por que precisaria cortar vértices repetidos? armazenar só os pushes já cortaria?)

    }

}

int main () {
    fstream tsp("data/tsp2_1248.txt", fstream::in);
    vector<vector<int>> graph = buildGraph(tsp);
    vector<vector<int>> tree = prim_mst(graph.size(), graph);
    vector<int> path;

    dfs(tree, path);
}