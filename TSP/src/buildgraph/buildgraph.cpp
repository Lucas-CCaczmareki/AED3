#include "buildgraph.h"
// #include <iostream> already declared in .h
#include <fstream>
#include <sstream>

std::vector<std::vector<int>> buildGraph(std::fstream& tsp) {
    std::vector<std::vector<int>> graph;    // dynamic matrix
    std::string line;                       // dynamic string

    if(!tsp.is_open()) {
        std::cout << "Erro ao abrir arquivo!";
    }

    // read the entire line of the file
    // breaks while with op bool() from the stream returned by getline when tries to read beyond EOF
    while (std::getline(tsp, line)) {
        std::istringstream strstream(line); //read string as stream
        graph.emplace_back(); //place a new vector object at the end of graph[0, 1, 2...]

        int x;
        while(strstream >> x) {
            // back goes to the last line
            // push.back puts the x into the end(back) of the last line.
            graph.back().push_back(x);
        }
    }

    // prints the graph matrix
    for(auto& l : graph) {
        std::cout << "\n";
        for(auto& x : l) {
            std::cout << x << "\t";
        }
    }

    return graph;
}
