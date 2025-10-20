// caida_libre_n_objetos_omp.cpp
// Compilación: g++ -O2 -fopenmp caida_libre_n_objetos_omp.cpp -o caida_omp
// Entrada (stdin):
//   N
//   (luego N alturas en metros, h >= 0)
// Salida:
//   N tiempos (segundos), uno por línea
// Ejemplo de entrada: ./caida_omp < alturas.txt

#include <bits/stdc++.h>
#include <omp.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    const double g = 9.80665;
    int N; 
    if (!(cin >> N)) return 0;

    vector<double> h(N), t(N);
    for (int i = 0; i < N; ++i) cin >> h[i];

    #pragma omp parallel for schedule(static)
    for (int i = 0; i < N; ++i) {
        t[i] = sqrt(2.0 * h[i] / g);
    }

    cout.setf(std::ios::fixed); cout << setprecision(6);
    for (int i = 0; i < N; ++i) cout << t[i] << "\n";
    return 0;
}
