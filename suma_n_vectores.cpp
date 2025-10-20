#include <iostream>
#include <string>

#define N 8 // Tamaño de cada vector 
#define NUM_VECTORS 3 // Número de vectores 

// ================= FUNCIÓN DE REDUCCIÓN (CPU) ===================
void reduceSum(const int *arr, int *result, int vectorIdx, int numVectors) {
    if (vectorIdx >= numVectors) return;

    int sum = 0;
    // Sumar todos los elementos del vector
    for (int i = 0; i < N; i++) {
        sum += arr[vectorIdx * N + i];
    }
    result[vectorIdx] = sum;
}

// ================= MAIN ===================
int main() {
    // Número de vectores hardcoded
    int numVectors = NUM_VECTORS;

    // Asignar memoria en el host
    int h_arr[N * numVectors]; // Arreglo para todos los vectores
    int h_result[numVectors];  // Arreglo para los resultados de cada vector

    //Se inicializa cada vector de manera diferente en el siguiente ciclo, por cada arreglo 
    //se agrega una nueva línea con la estructura h_arr[vectorIdx * N + i]
    for (int i = 0; i < N; i++) {
        h_arr[0 * N + i] = i + 1;  // Vector 0
        h_arr[1 * N + i] = 2 * (i + 1); // Vector 1
        h_arr[2 * N + i] = 10 + i; // Vector 2
    }

    // Procesar cada vector en la CPU
    for (int vectorIdx = 0; vectorIdx < numVectors; vectorIdx++) {
        reduceSum(h_arr, h_result, vectorIdx, numVectors);
    }

    // Imprimir resultados
    for (int i = 0; i < numVectors; i++) {
        std::cout << "Suma del vector " << i << ": " << h_result[i] << std::endl;
    }

    return 0;
}