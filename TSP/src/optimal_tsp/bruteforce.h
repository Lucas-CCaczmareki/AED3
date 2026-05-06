#ifndef BRUTE_FORCE_H
#define BRUTE_FORCE_H

#include <iostream>
#include <vector>
#include <fstream>
#include "../buildgraph/buildgraph.h"
#include <chrono>
#include <string>

using namespace std;

typedef struct {
    int cost;
    vector<int> path;
} bestPath;

void updateCost(vector<int>& v, bestPath& opt, vector<vector<int>>& graph);
void permute(vector<int>& v, int l, int r, bestPath& opt, vector<vector<int>>& graph);
void bruteforce( string data );

#endif