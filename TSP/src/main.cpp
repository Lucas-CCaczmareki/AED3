#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "optimal_tsp/bruteforce.h"
#include "approximate_tsp/mst_dfs.h"

string graph_menu();
int main_menu();

using namespace std;

int main() {

    for(;;) {
        int m = main_menu();
        switch (m)
        {
        case 1:{
            string data = graph_menu();
            cout << "\t### Brute Force ###\n";
            bruteforce(data);
        }
            break;
        
        case 2:{
            string data = graph_menu();
            cout << "\t### MST DFS ###\n";
            mst_dfs(data);
        }
            break;

        case 3:
            exit(0);
            break;
        }
    }
}

int main_menu() {
    int m = 0;

    do {
        cout << "\t### Escolha o algoritmo ###\n";
        cout <<"\t1.Exato - Brute Force\n";
        cout << "\t2.Aproximado - MST DFS\n";
        cout << "\t3.Sair\n";
        cout << "Escolha: ";
        cin >> m;
        getchar();

        if ( m <= 0 || m > 3 ) {
            cout << "Numero invalido, tente novamente\n";
        }
    }while( m <= 0 || m > 3 ); 

    return m;
}

string graph_menu() {
    int m = 0;

    for(;;) {
        do {
            cout << "\t### Escolha o algoritmo ###\n";
            cout << "\t1.TSP1 253\n";
            cout << "\t2.TSP2 1248\n";
            cout << "\t3.TSP3 1194\n";
            cout << "\t4.TSP4 7013\n";
            cout << "\t5.TSP5 27603\n";
            cout << "Escolha: ";
            cin >> m;
            getchar();

            if ( m <= 0 || m > 6 ) {
                cout << "Numero invalido, tente novamente\n";
            }
        }while( m <= 0 || m > 6 ); 

        switch (m)
        {
        case 1:
            return "data/tsp1_253.txt";
            break;
        
        case 2:
            return "data/tsp2_1248.txt";
            break;

        case 3:
            return "data/tsp3_1194.txt";
            break;

        case 4:
            return "data/tsp4_7013.txt";
            break;

        case 5:
            return "data/tsp5_27603.txt";
            break;
        }
    }
}

/*
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
*/