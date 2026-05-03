#include "dfs.h"

/*
ATENÇÃO!!!!
TREE NO PARÂMETRO É REPRESENTADA ATRAVÉS DE UMA LISTA DE ADJACÊNCIA
*/
void dfs(int node, vector<vector<int>> tree, vector<int>& path, vector<bool>& visited) {
    // Na primeira chamada recebe a origem da árvore. Pode ser arbitrariamente o 0, ou randômico. 
    // A responsabilidade de decidir isso fica pra main.

    if(!visited[node]) { // se o vértice q eu to ainda n foi explorado
        
        visited[node] = true;   // explora
        path.push_back(node);   // salva caminho

        // desce por todos filhos i do nó atual
        for(int i : tree[node]) {
            dfs(i, tree, path, visited);
        }
        
    }
    //ignora se ja foi visitado
}

/* //exemplo de main pra rodar só o dfs e fazer testes
// int main () {
//     fstream tsp("data/tsp2_1248.txt", fstream::in);
//     vector<vector<int>> graph = buildGraph(tsp);
//     vector<vector<int>> tree = prim_mst(graph.size(), graph);
//     vector<int> path;

//     //vou escolher arbitrariamente a origem como 0. Mas poderia ser randômica 
//     int origin_idx = 5; // usando 5 pq o exemplo funcionou melhor

//     // cria uma estrutura com visiteds
//     vector<bool> visited; 

//     //procurar um jeito melhor de inicializar depois
//     for(int i = 0; i < tree.size(); i++) {
//         visited.push_back(false);
//     }
//     //por enquanto vou fazer começando fixo no 0
//     // visited[origin_idx] = true; 


//     dfs(3, tree, path, visited);

//     for(int x : path) {
//         cout << x << " ";
//     }
//     cout << "\n";
// }

*/
