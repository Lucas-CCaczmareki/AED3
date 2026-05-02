#include "prim_mst.h"
using namespace std;

// returns an adjacency list || nodes = number of nodes in the graph
vector<vector<int>> mst(int nodes, vector<vector<int>> graph) {    
    vector<vector<int>> adjList;      // adjacency list
    vector<bool> visited(nodes, false);    // visited nodes

    // setting  the priority queue to organize the paths, from least expensive to the most expensive
    priority_queue< 
        tuple<int, int, int>,      // cost, origin, destination
        vector<tuple<int, int, int>>,
        greater<tuple<int, int, int>>
    > queue;


    return adjList;
}

/*
### Things to remember ###

- edge = nodes - 1
- the priority queue should be from least to most
*/
