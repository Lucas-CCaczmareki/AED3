#ifndef MST_DFS_H
#define MST_DFS_H

#include "../dfs/dfs.h"
#include "../buildgraph/buildgraph.h"
#include "../prim_mst/prim_mst.h"
#include <fstream>
#include <random>
#include <chrono>
#include <string>

using namespace std;

double approximate( vector<int> path, vector<vector<int>> graph );
void mst_dfs( string data );

#endif