#include "MatrixMultiplier.h"
#include "Utils.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <iomanip>
#include <cmath>

MatrixMultiplier::MatrixMultiplier(int size, int block_size)
    : size(size), block_size(block_size), matrixA(nullptr), matrixB(nullptr), matrixC(nullptr) {
}

MatrixMultiplier::~MatrixMultiplier() {
    if (matrixA) free_matrix(matrixA, size);
    if (matrixB) free_matrix(matrixB, size);
    if (matrixC) free_matrix(matrixC, size);
}

void MatrixMultiplier::initialize_matrices() {
    // Crear matrices
    matrixA = create_matrix(size, size);
    matrixB = create_matrix(size, size);
    matrixC = create_matrix(size, size);

    // Inicializar con valores aleatorios
    srand(time(nullptr));
    randomize_matrix(matrixA, size, size);
    randomize_matrix(matrixB, size, size);
    zero_matrix(matrixC, size, size);
}

void MatrixMultiplier::clear_result() {
    if (matrixC) {
        zero_matrix(matrixC, size, size);
    }
}

void MatrixMultiplier::multiply_block(int i_start, int j_start, int k_start, int bs) {
    /**
     * Multiplicación de un bloque específico.
     *
     * Este es el núcleo de la optimización por bloques:
     * - Procesa sub-matrices pequeñas que caben en caché L1
     * - Reduce cache misses dramáticamente
     * - Mejora localidad espacial y temporal
     *
     * Para cada elemento C[i][j] en el bloque:
     *   C[i][j] += sum(A[i][k] * B[k][j]) para k en [k_start, k_start+bs)
     */

    int i_end = std::min(i_start + bs, size);
    int j_end = std::min(j_start + bs, size);
    int k_end = std::min(k_start + bs, size);

    for (int i = i_start; i < i_end; i++) {
        for (int j = j_start; j < j_end; j++) {
            double sum = 0.0;
            for (int k = k_start; k < k_end; k++) {
                sum += matrixA[i][k] * matrixB[k][j];
            }
            matrixC[i][j] += sum;
        }
    }
}

void MatrixMultiplier::multiply_sequential() {
    /**
     * Multiplicación secuencial optimizada por bloques (Blocking/Tiling)
     *
     * Algoritmo:
     * 1. Dividir cada matriz en bloques de tamaño block_size x block_size
     * 2. Para cada bloque de C:
     *    - Calcular usando bloques correspondientes de A y B
     * 3. Cada bloque cabe en caché L1, minimizando cache misses
     *
     * Complejidad: O(N^3) igual que algoritmo estándar
     * Mejora: 5-20x más rápido debido a mejor uso de caché
     *
     * Visualización (ejemplo 8x8 con bloques 4x4):
     *
     * C = A * B
     *
     * ┌───┬───┐   ┌───┬───┐   ┌───┬───┐
     * │ 0 │ 1 │   │ 0 │ 1 │   │ 0 │ 1 │
     * ├───┼───┤ = ├───┼───┤ × ├───┼───┤
     * │ 2 │ 3 │   │ 2 │ 3 │   │ 2 │ 3 │
     * └───┴───┘   └───┴───┘   └───┴───┘
     *
     * Cada cuadro numerado es un bloque 4x4
     */

    // Iterar por bloques
    for (int i = 0; i < size; i += block_size) {
        for (int j = 0; j < size; j += block_size) {
            for (int k = 0; k < size; k += block_size) {
                // Multiplicar bloque actual
                multiply_block(i, j, k, block_size);
            }
        }
    }
}

double** MatrixMultiplier::get_matrix_a() {
    return matrixA;
}

double** MatrixMultiplier::get_matrix_b() {
    return matrixB;
}

double** MatrixMultiplier::get_matrix_c() {
    return matrixC;
}

int MatrixMultiplier::get_size() const {
    return size;
}

int MatrixMultiplier::get_block_size() const {
    return block_size;
}

int MatrixMultiplier::get_num_blocks() const {
    return (size + block_size - 1) / block_size;
}

void MatrixMultiplier::print_block_info() const {
    int num_blocks = get_num_blocks();
    int total_blocks = num_blocks * num_blocks;

    std::cout << "\n=== CONFIGURACION DE BLOQUES ===" << std::endl;
    std::cout << std::left;
    std::cout << std::setw(30) << "Tamanio de matriz:" << size << "x" << size << std::endl;
    std::cout << std::setw(30) << "Tamanio de bloque:" << block_size << "x" << block_size
              << " (auto-optimizado)" << std::endl;
    std::cout << std::setw(30) << "Bloques por dimension:" << num_blocks << "x" << num_blocks << std::endl;
    std::cout << std::setw(30) << "Total de bloques:" << total_blocks << std::endl;

    // Visualización simple de bloques (solo si no es muy grande)
    if (num_blocks <= 16) {
        std::cout << "\nVisualizacion (cada [] = 1 bloque de " << block_size << "x" << block_size << "):\n";
        for (int i = 0; i < num_blocks; i++) {
            for (int j = 0; j < num_blocks; j++) {
                std::cout << "[] ";
            }
            std::cout << std::endl;
        }
    }

    Utils::print_separator();
}

// Funciones estáticas auxiliares

double** MatrixMultiplier::create_matrix(int rows, int cols) {
    double** matrix = new double*[rows];
    for (int i = 0; i < rows; i++) {
        matrix[i] = new double[cols];
    }
    return matrix;
}

void MatrixMultiplier::free_matrix(double** matrix, int rows) {
    if (matrix) {
        for (int i = 0; i < rows; i++) {
            delete[] matrix[i];
        }
        delete[] matrix;
    }
}

void MatrixMultiplier::randomize_matrix(double** matrix, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            matrix[i][j] = static_cast<double>(rand() % 10);
        }
    }
}

void MatrixMultiplier::zero_matrix(double** matrix, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            matrix[i][j] = 0.0;
        }
    }
}
