#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <string>

using namespace std;

// ================= SOBEL FILTER (CPU) ===================
void sobelFilter(const unsigned char* input, unsigned char* output, int width, int height) {
    int sobel_x[3][3] = {{-1, 0, 1},
                         {-2, 0, 2},
                         {-1, 0, 1}};

    int sobel_y[3][3] = {{-1, -2, -1},
                         { 0,  0,  0},
                         { 1,  2,  1}};

    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            int gx = 0, gy = 0;

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

    if (magic == "P2") {  // PGM (grises)
        img.resize(width * height);
        file.read((char*)img.data(), img.size());
    } 
    else if (magic == "P3") { // PPM (color)
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
    if (argc < 2) {
        cerr << "Uso: " << argv[0] << " input1.ppm|pgm [input2.ppm|pgm ...]\n";
        cerr << "Cada salida se guardará como inputX_sobel.pgm\n";
        return 1;
    }

    for (int i = 1; i < argc; ++i) {
        string inputFile = argv[i];

        // Generar nombre de salida: agregar "_sobel" antes de la extensión
        size_t dotPos = inputFile.find_last_of('.');
        string outputFile = (dotPos != string::npos) ?
            inputFile.substr(0, dotPos) + "_sobel.pgm" :
            inputFile + "_sobel.pgm";

        vector<unsigned char> img;
        int width, height;

        if (!readImage(inputFile, img, width, height)) {
            cerr << "Error leyendo " << inputFile << endl;
            continue;  // Continuar con la siguiente imagen si hay error
        }

        // Crear buffer de salida
        vector<unsigned char> result(width * height, 0); // Inicializar a 0

        // Aplicar filtro Sobel en CPU
        sobelFilter(img.data(), result.data(), width, height);

        // Guardar resultado
        writePGM(outputFile, result, width, height);

        cout << "✅ Imagen procesada y guardada en " << outputFile << endl;
    }

    return 0;
}
