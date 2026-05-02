#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "buildgraph/buildgraph.h"
#include "prim_mst/prim_mst.h"

int main (void) {
    // std::cout << "Hello World. Testing TSP!\n 'Optimal' solution...";

    std::fstream tsp ("data/tsp1_253.txt", std::fstream::in);
    vector<vector<int>> graph = buildGraph(tsp);
    vector<vector<int>> tree = prim_mst(graph.size(), graph);

}