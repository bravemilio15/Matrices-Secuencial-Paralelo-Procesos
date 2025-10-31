#include "ProcessManager.h"
#include <iostream>
#include <cstring>
#include <algorithm>

// Headers específicos de plataforma
#ifdef _WIN32
    #include <windows.h>
    #include <process.h>
#else
    #include <unistd.h>
    #include <sys/wait.h>
#endif

ProcessManager::ProcessManager(int matrix_size, int block_size, int num_processes)
    : matrix_size(matrix_size), block_size(block_size), num_processes(num_processes) {
}

ProcessManager::~ProcessManager() {
    // Los unique_ptr se encargan de liberar SharedMemory automáticamente
}

void ProcessManager::setup_shared_memory(double** matrixA, double** matrixB) {
    size_t matrix_bytes = matrix_size * matrix_size * sizeof(double);

    // Crear memoria compartida para cada matriz
    shm_A = std::make_unique<SharedMemory>("matrix_A", matrix_bytes, true);
    shm_B = std::make_unique<SharedMemory>("matrix_B", matrix_bytes, true);
    shm_C = std::make_unique<SharedMemory>("matrix_C", matrix_bytes, true);

    if (!shm_A->is_valid() || !shm_B->is_valid() || !shm_C->is_valid()) {
        std::cerr << "Error: No se pudo crear memoria compartida" << std::endl;
        return;
    }

    // Copiar datos de matrices a memoria compartida
    double* shared_A = static_cast<double*>(shm_A->get_ptr());
    double* shared_B = static_cast<double*>(shm_B->get_ptr());
    double* shared_C = static_cast<double*>(shm_C->get_ptr());

    // Copiar A y B a memoria compartida (layout contiguo)
    for (int i = 0; i < matrix_size; i++) {
        std::memcpy(&shared_A[i * matrix_size], matrixA[i], matrix_size * sizeof(double));
        std::memcpy(&shared_B[i * matrix_size], matrixB[i], matrix_size * sizeof(double));
    }

    // Inicializar C en cero
    std::memset(shared_C, 0, matrix_bytes);
}

void ProcessManager::copy_result_from_shared(double** matrixC) {
    if (!shm_C->is_valid()) {
        std::cerr << "Error: Memoria compartida C no válida" << std::endl;
        return;
    }

    double* shared_C = static_cast<double*>(shm_C->get_ptr());

    // Copiar resultado de memoria compartida a matriz local
    for (int i = 0; i < matrix_size; i++) {
        std::memcpy(matrixC[i], &shared_C[i * matrix_size], matrix_size * sizeof(double));
    }
}

void ProcessManager::worker_process(int block_start, int block_end) {
    /**
     * Función ejecutada por cada proceso hijo.
     *
     * Cada proceso calcula un rango de bloques de la matriz resultado.
     * Los bloques se numeran linealmente: 0, 1, 2, ..., total_blocks-1
     *
     * Para un bloque lineal 'b', se convierte a coordenadas (bi, bj):
     *   bi = b / num_blocks_per_dim
     *   bj = b % num_blocks_per_dim
     *
     * Luego se calculan las coordenadas reales en la matriz:
     *   i = bi * block_size
     *   j = bj * block_size
     */

    double* shared_A = static_cast<double*>(shm_A->get_ptr());
    double* shared_B = static_cast<double*>(shm_B->get_ptr());
    double* shared_C = static_cast<double*>(shm_C->get_ptr());

    int num_blocks_per_dim = (matrix_size + block_size - 1) / block_size;

    // Procesar cada bloque asignado a este proceso
    for (int block_idx = block_start; block_idx < block_end; block_idx++) {
        // Convertir índice lineal a coordenadas 2D
        int bi = block_idx / num_blocks_per_dim;
        int bj = block_idx % num_blocks_per_dim;

        int i_start = bi * block_size;
        int j_start = bj * block_size;

        // Multiplicar este bloque
        for (int k = 0; k < matrix_size; k += block_size) {
            int i_end = std::min(i_start + block_size, matrix_size);
            int j_end = std::min(j_start + block_size, matrix_size);
            int k_end = std::min(k + block_size, matrix_size);

            // Triple loop para el bloque
            for (int i = i_start; i < i_end; i++) {
                for (int j = j_start; j < j_end; j++) {
                    double sum = 0.0;
                    for (int kk = k; kk < k_end; kk++) {
                        sum += shared_A[i * matrix_size + kk] * shared_B[kk * matrix_size + j];
                    }
                    shared_C[i * matrix_size + j] += sum;
                }
            }
        }
    }
}

std::vector<std::pair<int, int>> ProcessManager::calculate_block_distribution() const {
    int total_blocks = get_total_blocks();
    std::vector<std::pair<int, int>> distribution;

    int blocks_per_process = total_blocks / num_processes;
    int remaining_blocks = total_blocks % num_processes;

    int current_block = 0;
    for (int p = 0; p < num_processes; p++) {
        int start = current_block;
        int count = blocks_per_process + (p < remaining_blocks ? 1 : 0);
        int end = start + count;

        distribution.push_back({start, end});
        current_block = end;
    }

    return distribution;
}

int ProcessManager::get_total_blocks() const {
    int num_blocks_per_dim = (matrix_size + block_size - 1) / block_size;
    return num_blocks_per_dim * num_blocks_per_dim;
}

void ProcessManager::multiply_parallel(double** matrixA, double** matrixB, double** matrixC) {
    /**
     * Multiplicación paralela usando procesos.
     *
     * Estrategia de distribución:
     * 1. Calcular número total de bloques
     * 2. Distribuir bloques equitativamente entre procesos
     * 3. Cada proceso calcula su rango de bloques
     *
     * Sincronización:
     * - Linux/macOS: waitpid() espera a cada hijo
     * - Windows: WaitForMultipleObjects()
     */

    // Configurar memoria compartida
    setup_shared_memory(matrixA, matrixB);

    // Calcular distribución de bloques
    auto distribution = calculate_block_distribution();

    #ifndef _WIN32
        // Implementación POSIX (Linux/macOS) con fork()
        std::vector<pid_t> child_pids;

        for (int p = 0; p < num_processes; p++) {
            pid_t pid = fork();

            if (pid == -1) {
                std::cerr << "Error al crear proceso hijo" << std::endl;
                continue;
            } else if (pid == 0) {
                // Proceso hijo
                int block_start = distribution[p].first;
                int block_end = distribution[p].second;

                worker_process(block_start, block_end);

                // Terminar proceso hijo
                exit(0);
            } else {
                // Proceso padre: guardar PID del hijo
                child_pids.push_back(pid);
            }
        }

        // Proceso padre: esperar a todos los hijos
        for (pid_t pid : child_pids) {
            int status;
            waitpid(pid, &status, 0);
        }

    #else
        // Implementación Windows con CreateProcess
        // (Simplificada - en producción se usaría CreateProcess con parámetros)
        std::cerr << "Advertencia: Implementación Windows no completada en esta versión" << std::endl;
        std::cerr << "Ejecutando versión secuencial como fallback" << std::endl;

        // Fallback: ejecutar secuencialmente
        for (int p = 0; p < num_processes; p++) {
            worker_process(distribution[p].first, distribution[p].second);
        }
    #endif

    // Copiar resultado de memoria compartida a matriz local
    copy_result_from_shared(matrixC);
}
