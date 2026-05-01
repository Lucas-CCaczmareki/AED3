#include <iostream>
#include <vector>
using namespace std; //importa o lugar onde ficam as funções std ai n precisa ficar usando std::

void permute(vector<int>& v, int l, int r) {
    if (l == r) {
        for (int x : v) cout << x << " ";
        cout << "\n";
        return;
    }

    for (int i = l; i <= r; i++) {
        swap(v[l], v[i]);
        permute(v, l + 1, r);
        swap(v[l], v[i]); // volta (backtrack)
    }
}

int main() {
    vector<int> v = {0, 1, 2};
    permute(v, 0, 2);
}