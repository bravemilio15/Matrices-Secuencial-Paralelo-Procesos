#ifndef MATRIX_MULTIPLIER_H
#define MATRIX_MULTIPLIER_H

#include <vector>

/**
 * @brief Clase para realizar multiplicación de matrices optimizada
 *
 * Implementa dos algoritmos:
 * 1. Multiplicación secuencial con bloques (cache-friendly)
 * 2. Multiplicación paralela con procesos
 *
 * La optimización por bloques mejora significativamente el rendimiento
 * al aprovechar la localidad de caché.
 */
class MatrixMultiplier {
private:
    int size;           // Tamaño de las matrices (N x N)
    int block_size;     // Tamaño de bloque para tiling

    double** matrixA;
    double** matrixB;
    double** matrixC;

    /**
     * @brief Multiplica dos bloques de matrices
     *
     * Realiza C[i:i+block][j:j+block] += A[i:i+block][k:k+block] * B[k:k+block][j:j+block]
     *
     * @param i_start Fila inicial del bloque
     * @param j_start Columna inicial del bloque
     * @param k_start Índice k inicial
     * @param block_size Tamaño del bloque
     */
    void multiply_block(int i_start, int j_start, int k_start, int block_size);

public:
    /**
     * @brief Constructor
     * @param size Tamaño de las matrices (N x N)
     * @param block_size Tamaño del bloque para optimización
     */
    MatrixMultiplier(int size, int block_size);

    /**
     * @brief Destructor - libera memoria de matrices
     */
    ~MatrixMultiplier();

    /**
     * @brief Crea e inicializa las matrices con valores aleatorios
     */
    void initialize_matrices();

    /**
     * @brief Limpia la matriz resultado C (pone todos sus valores en 0)
     */
    void clear_result();

    /**
     * @brief Multiplicación secuencial optimizada por bloques
     *
     * Algoritmo:
     * - Divide las matrices en bloques de tamaño block_size x block_size
     * - Para cada bloque, realiza la multiplicación aprovechando caché L1
     * - Mejora significativa vs. algoritmo triple-loop estándar
     *
     * Complejidad: O(N^3) pero con mejor constante por cache locality
     */
    void multiply_sequential();

    /**
     * @brief Obtiene puntero a matriz A
     */
    double** get_matrix_a();

    /**
     * @brief Obtiene puntero a matriz B
     */
    double** get_matrix_b();

    /**
     * @brief Obtiene puntero a matriz C (resultado)
     */
    double** get_matrix_c();

    /**
     * @brief Obtiene tamaño de las matrices
     */
    int get_size() const;

    /**
     * @brief Obtiene tamaño de bloque usado
     */
    int get_block_size() const;

    /**
     * @brief Imprime información sobre la configuración de bloques
     */
    void print_block_info() const;

    /**
     * @brief Calcula el número total de bloques
     * @return Número de bloques por dimensión
     */
    int get_num_blocks() const;

    // Funciones auxiliares estáticas

    /**
     * @brief Crea una matriz dinámica de tamaño rows x cols
     * @return Puntero a matriz
     */
    static double** create_matrix(int rows, int cols);

    /**
     * @brief Libera memoria de una matriz
     * @param matrix Puntero a la matriz
     * @param rows Número de filas
     */
    static void free_matrix(double** matrix, int rows);

    /**
     * @brief Inicializa matriz con valores aleatorios entre 0 y 9
     * @param matrix Puntero a la matriz
     * @param rows Número de filas
     * @param cols Número de columnas
     */
    static void randomize_matrix(double** matrix, int rows, int cols);

    /**
     * @brief Pone todos los valores de una matriz en 0
     * @param matrix Puntero a la matriz
     * @param rows Número de filas
     * @param cols Número de columnas
     */
    static void zero_matrix(double** matrix, int rows, int cols);
};

#endif // MATRIX_MULTIPLIER_H
