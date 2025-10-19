#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <cuda_runtime.h>

using namespace std;


// ================= CUDA KERNEL ===================
__global__ void sobelKernel(unsigned char* input, unsigned char* output, int width, int height) {
    int x = blockIdx.x * blockDim.x + threadIdx.x; 
    int y = blockIdx.y * blockDim.y + threadIdx.y;

    if (x > 0 && y > 0 && x < width - 1 && y < height - 1) {
        int gx = 0, gy = 0;

        int sobel_x[3][3] = {{-1, 0, 1},
                             {-2, 0, 2},
                             {-1, 0, 1}};

        int sobel_y[3][3] = {{-1, -2, -1},
                             { 0,  0,  0},
                             { 1,  2,  1}};

        for (int i = -1; i <= 1; i++) {
            for (int j = -1; j <= 1; j++) {
                int pixel = input[(y + i) * width + (x + j)];
                gx += pixel * sobel_x[i + 1][j + 1];
                gy += pixel * sobel_y[i + 1][j + 1];
            }
        }

        int val = (int)sqrtf((float)(gx * gx + gy * gy));
        if (val > 255) val = 255;
        if (val < 0) val = 0;
        output[y * width + x] = (unsigned char)val;
    }
}

// ================= PGM/PPM READER ===================
bool readImage(const string& filename, vector<unsigned char>& img, int& width, int& height) {
    ifstream file(filename, ios::binary);
    if (!file.is_open()) return false;

    string magic;
    file >> magic;
    file >> width >> height;
    int maxval;
    file >> maxval;
    file.get(); // saltar salto de línea

    if (magic == "P5") {  // PGM (grises)
        img.resize(width * height);
        file.read((char*)img.data(), img.size());
    } 
    else if (magic == "P6") { // PPM (color)
        vector<unsigned char> temp(width * height * 3);
        file.read((char*)temp.data(), temp.size());
        img.resize(width * height);
        for (int i = 0; i < width * height; i++) {
            // Conversión a grises (promedio)
            img[i] = (temp[3*i] + temp[3*i+1] + temp[3*i+2]) / 3;
        }
    } 
    else {
        cerr << "Formato no soportado: " << magic << endl;
        return false;
    }
    return true;
}

// ================= PGM WRITER ===================
void writePGM(const string& filename, const vector<unsigned char>& img, int width, int height) {
    ofstream file(filename, ios::binary);
    file << "P5\n" << width << " " << height << "\n255\n";
    file.write((char*)img.data(), img.size());
}

// ================= MAIN ===================
int main(int argc, char* argv[]) {
    if (argc < 3) {
        cerr << "Uso: " << argv[0] << " input.ppm|pgm output.pgm\n";
        return 1;
    }

    string inputFile = argv[1];
    string outputFile = argv[2];

    vector<unsigned char> img;
    int width, height;

    if (!readImage(inputFile, img, width, height)) {
        cerr << "Error leyendo " << inputFile << endl;
        return 1;
    }

    // Reservar memoria en GPU
    unsigned char *d_input, *d_output;
    size_t size = width * height * sizeof(unsigned char);
    cudaMalloc(&d_input, size);
    cudaMalloc(&d_output, size);

    cudaMemcpy(d_input, img.data(), size, cudaMemcpyHostToDevice);

    dim3 threads(16, 16);
    dim3 blocks((width + threads.x - 1) / threads.x, 
                (height + threads.y - 1) / threads.y);

    sobelKernel<<<blocks, threads>>>(d_input, d_output, width, height);
    cudaDeviceSynchronize();

    vector<unsigned char> result(width * height);
    cudaMemcpy(result.data(), d_output, size, cudaMemcpyDeviceToHost);

    writePGM(outputFile, result, width, height);

    cudaFree(d_input);
    cudaFree(d_output);

    cout << "✅ Imagen procesada y guardada en " << outputFile << endl;
    return 0;
}