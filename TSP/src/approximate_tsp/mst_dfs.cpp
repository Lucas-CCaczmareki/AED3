#include "../dfs/dfs.h"
#include "../buildgraph/buildgraph.h"
#include "../prim_mst/prim_mst.h"
#include <fstream>
#include <random>

int main () {
    fstream tsp("data/tsp2_1248.txt", fstream::in);
    vector<vector<int>> graph = buildGraph(tsp);
    vector<vector<int>> tree = prim_mst(graph.size(), graph);
    vector<int> path;

    //vou escolher arbitrariamente a origem como 5. Mas poderia ser randômica 
    int origin_idx = 5; // usando 5 pq o exemplo funcionou melhor

    // caso queira origem randômica
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distrib(0, (tree.size() - 1));
    origin_idx = distrib(gen);
    cout << "origem aleatória: " << origin_idx << "\n";

    // cria uma estrutura com visiteds
    vector<bool> visited; 

    //procurar um jeito melhor de inicializar depois
    for(int i = 0; i < tree.size(); i++) {
        visited.push_back(false);
    }

    dfs(origin_idx, tree, path, visited);

    // printa o caminho do dfs
    for(int x : path) {
        cout << x << " ";
    }
    cout << "\n";
}