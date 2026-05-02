#include "buildgraph.h"
// #include <iostream> already declared in .h
#include <fstream>
#include <sstream>
using namespace std;

vector<vector<int>> buildGraph(fstream& tsp) {
    vector<vector<int>> graph;    // dynamic matrix
    string line;                       // dynamic string

    if(!tsp.is_open()) {
        cout << "Erro ao abrir arquivo!\n";
    }

    // read the entire line of the file
    // breaks while with op bool() from the stream returned by getline when tries to read beyond EOF
    while (getline(tsp, line)) {
        istringstream strstream(line); //read string as stream
        graph.emplace_back(); //place a new vector object at the end of graph[0, 1, 2...]

        int x;
        while(strstream >> x) {
            // back goes to the last line
            // push.back puts the x into the end(back) of the last line.
            graph.back().push_back(x);
        }
    }

    // prints the graph matrix
    // for(auto& l : graph) {
    //     cout << "\n";
    //     for(auto& x : l) {
    //         cout << x << "\t";
    //     }
    // }

    return graph;
}
