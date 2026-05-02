#include "prim_mst.h"

std::vector<std::vector<int>> mst( int nodes, std::vector<std::vector<int>> graph ) {    // returns an adjacency list || nodes = number of nodes in the graph
    std::vector<std::vector<int>> adjList;      // adjacency list
    std::vector<bool> visited(nodes, false);    // visited nodes

    // setting  the priority queue to organize the paths, from least expensive to the most expensive
    std::priority_queue< 
        std::tuple<int, int, int>,      // cost, origin, destination
        std::vector<std::tuple<int, int, int>>,
        std::greater<std::tuple<int, int, int>>
        > queue;


    return adjList;
}

/*
### Things to remember ###

- edge = nodes - 1
- the priority queue should be from least to most
*/
