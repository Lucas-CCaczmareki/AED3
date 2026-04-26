#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "buildgraph/buildgraph.h"

int main (void) {
    // std::cout << "Hello World. Testing TSP!\n 'Optimal' solution...";

    std::fstream tsp ("data/tsp1_253.txt", std::fstream::in);
    buildGraph(tsp);
    

}