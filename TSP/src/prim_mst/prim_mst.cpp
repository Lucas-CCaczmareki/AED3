#include "prim_mst.h"
using namespace std;

vector<vector<int>> prim_mst( int nodes, vector<vector<int>>& graph ) {    // returns an adjacency list || nodes = number of nodes in the graph
    vector<vector<int>> adjList(nodes);     // adjacency list
    vector<bool> visited(nodes, false);     // visited nodes

    // setting the priority queue to organize the paths, from least expensive to the most expensive
    priority_queue< 
        tuple<int, int, int>,               // cost, origin, destination
        vector<tuple<int, int, int>>,
        greater<tuple<int, int, int>>
        > queue;

    visited[0] = true;                      // marca o primeiro nodo como visitado
    for( int i = 0; i < nodes; ++i ) {      // coloca todas as ligações do primeiro nodo na fila
        if ( graph[0][i] != 0 ) {
            queue.push({ graph[0][i], 0, i });
        }
    }

    int edges = 0;                                      // inicia o numero de arestas da MST em 0
    while( edges < (nodes - 1) && !queue.empty() ) {    // enquanto a fila n estiver vazia e o nº de arestas for menor q o nº de vertices, executa
        // tira o primeiro da fila
        auto tupla = queue.top(); 
        int dest = get<2>(tupla);
        queue.pop();

        // se o nodo retirado já tiver sido visitado, continua para a próxima execução do while, se não, o msm while continua
        if( visited[dest] ) {
            continue;
        }

        // marca o nodo como visitado e coloca a aresta na lista de adjacencia 
        visited[dest] = true;       
        int origin = get<1>(tupla);
        adjList[origin].push_back(dest);
        adjList[dest].push_back(origin);
        ++edges;

        // coloca todas as ligações do nodo na fila
        for( int i = 0; i < nodes; ++i ) {
            if ( graph[dest][i] != 0 ) {
                queue.push({ graph[dest][i], dest, i });
            }
        }
    }
    
    return adjList;
}

/*
### Things to remember ###

- edge = nodes - 1
- the priority queue should be from least to most
*/
