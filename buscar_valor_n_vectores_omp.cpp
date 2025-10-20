// buscar_valor_n_vectores_omp.cpp
// Compilación: g++ -O2 -fopenmp buscar_valor_n_vectores_omp.cpp -o buscar_omp
// Entrada (stdin):
//   N M v
//   (luego N líneas con M enteros cada una)
// Salida:
//   N líneas con el índice de la primera aparición de v en cada vector (o -1)
// Ejemplo de entrada: ./buscar_omp < entrada_buscar.txt

#include <bits/stdc++.h>
#include <omp.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int N, M;
    long long v;
    if (!(cin >> N >> M >> v)) return 0;

    vector<vector<long long>> V(N, vector<long long>(M));
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < M; ++j)
            cin >> V[i][j];

    vector<int> idx(N, -1);

    #pragma omp parallel for schedule(static)
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < M; ++j) {
            if (V[i][j] == v) { idx[i] = j; break; }
        }
    }

    for (int i = 0; i < N; ++i) cout << idx[i] << "\n";
    return 0;
}