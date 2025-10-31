#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H

#include "MatrixMultiplier.h"
#include "SharedMemory.h"
#include <vector>
#include <memory>

/**
 * @brief Clase para gestionar la multiplicación paralela con procesos
 *
 * Implementa paralelización usando:
 * - Linux/macOS: fork() + memoria compartida (mmap)
 * - Windows: CreateProcess() + memoria compartida
 *
 * Distribuye el trabajo dividiendo la matriz en rangos de bloques
 */
class ProcessManager {
private:
    int matrix_size;
    int block_size;
    int num_processes;

    // Memoria compartida para las matrices
    std::unique_ptr<SharedMemory> shm_A;
    std::unique_ptr<SharedMemory> shm_B;
    std::unique_ptr<SharedMemory> shm_C;

    /**
     * @brief Configura matrices en memoria compartida
     * @param matrixA Matriz A fuente
     * @param matrixB Matriz B fuente
     */
    void setup_shared_memory(double** matrixA, double** matrixB);

    /**
     * @brief Copia matriz de memoria compartida a matriz local
     * @param matrixC Matriz destino
     */
    void copy_result_from_shared(double** matrixC);

    /**
     * @brief Función ejecutada por cada proceso hijo
     *
     * Calcula un rango de bloques de la matriz resultado
     *
     * @param block_start Bloque inicial a procesar
     * @param block_end Bloque final a procesar (exclusivo)
     */
    void worker_process(int block_start, int block_end);

public:
    /**
     * @brief Constructor
     * @param matrix_size Tamaño de las matrices
     * @param block_size Tamaño de bloque para optimización
     * @param num_processes Número de procesos a usar
     */
    ProcessManager(int matrix_size, int block_size, int num_processes);

    /**
     * @brief Destructor
     */
    ~ProcessManager();

    /**
     * @brief Ejecuta multiplicación paralela con procesos
     *
     * Algoritmo:
     * 1. Crear memoria compartida para A, B, C
     * 2. Calcular número total de bloques
     * 3. Dividir bloques entre procesos equitativamente
     * 4. Crear procesos con fork() (o CreateProcess en Windows)
     * 5. Cada proceso calcula su rango de bloques
     * 6. Proceso padre espera a todos los hijos (wait/waitpid)
     * 7. Copiar resultado de memoria compartida
     *
     * @param matrixA Matriz A (entrada)
     * @param matrixB Matriz B (entrada)
     * @param matrixC Matriz C (resultado - será sobreescrita)
     */
    void multiply_parallel(double** matrixA, double** matrixB, double** matrixC);

    /**
     * @brief Calcula el número total de bloques
     * @return Número total de bloques (num_blocks_x * num_blocks_y)
     */
    int get_total_blocks() const;

    /**
     * @brief Calcula la distribución de bloques por proceso
     * @return Vector con [inicio, fin) de bloques para cada proceso
     */
    std::vector<std::pair<int, int>> calculate_block_distribution() const;
};

#endif // PROCESS_MANAGER_H
