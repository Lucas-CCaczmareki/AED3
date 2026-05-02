#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "buildgraph/buildgraph.h"
#include "prim_mst/prim_mst.h"

using namespace std;

int main (void) {
    // std::cout << "Hello World. Testing TSP!\n 'Optimal' solution...";

    fstream tsp ("data/tsp2_1248.txt", fstream::in);
    vector<vector<int>> graph = buildGraph(tsp);
    vector<vector<int>> tree = prim_mst(graph.size(), graph);

    int i = 0;
    for(vector<int> connections : tree) {
        cout << i << " :";
        for(int j = 0; j < connections.size(); j++) {
            cout << " " << connections[j];
        }
        cout << "\n";
        ++i;
    }

}