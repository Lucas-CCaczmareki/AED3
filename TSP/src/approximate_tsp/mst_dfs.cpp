#include "mst_dfs.h"

double approximate( vector<int> path, vector<vector<int>> graph );

void mst_dfs( string data ) {
    fstream tsp(data, fstream::in);
    vector<vector<int>> graph = buildGraph(tsp);
    vector<vector<int>> tree = prim_mst(graph.size(), graph);
    vector<int> path;

    // Origem arbitrária iniciando em 0
    int origin_idx = 0; 
    cout << "Origem arbitrária: " << origin_idx << "\n";

    // caso queira origem randômica
    // random_device rd;
    // mt19937 gen(rd());
    // uniform_int_distribution<> distrib(0, (tree.size() - 1));
    // origin_idx = distrib(gen);
    // cout << "Origem aleatoria: " << origin_idx << "\n";

    // cria uma estrutura com visiteds
    vector<bool> visited(tree.size(), false); 

    auto init = chrono::high_resolution_clock::now(); // inicia contagem de tempo

    dfs(origin_idx, tree, path, visited);
    double cost = approximate( path, graph );

    auto res = chrono::high_resolution_clock::now() - init; // finaliza contagem de tempo

    long long microseconds = chrono::duration_cast<chrono::microseconds>(res).count();
    cout << "tempo de exucação: " << microseconds << "µs\n";

    // printa o caminho do dfs
    cout << "Path: ";
    for(int x : path) {
        cout << x << " ";
    }
    cout << "\n";

    

    cout << "Cost: " << cost;
    cout << "\n";

    tsp.close();
}

double approximate( vector<int> path, vector<vector<int>> graph ) {
    double cost = 0;
    int i, origin = path[0];

    // soma os custos dos caminhos
    for( i = 0; i < (path.size() - 1); ++i) {
        cost += graph[path[i]][path[i + 1]];
    }
    // finaliza somando o custo do caminho do último vértice até o primeiro
    cost += graph[origin][path[i]];

    return cost;
}