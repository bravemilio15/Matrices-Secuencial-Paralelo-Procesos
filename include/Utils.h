#ifndef UTILS_H
#define UTILS_H

#include <chrono>
#include <string>
#include <vector>

/**
 * @brief Clase para medir tiempo de ejecución con precisión
 *
 * Usa std::chrono para medición de alta resolución.
 * Patrón RAII: inicia al construir, para al destruir o llamar a stop()
 */
class Timer {
private:
    std::chrono::high_resolution_clock::time_point start_time;
    std::chrono::high_resolution_clock::time_point end_time;
    bool is_running;

public:
    Timer();
    void start();
    void stop();
    double elapsed_seconds() const;
    double elapsed_milliseconds() const;
};

/**
 * @brief Funciones de utilidad para formateo y visualización
 */
namespace Utils {
    /**
     * @brief Imprime una línea separadora
     * @param length Longitud de la línea
     * @param character Carácter a usar
     */
    void print_separator(int length = 60, char character = '=');

    /**
     * @brief Imprime un encabezado centrado
     * @param title Título a imprimir
     * @param width Ancho total
     */
    void print_header(const std::string& title, int width = 60);

    /**
     * @brief Calcula el siguiente número que es potencia de 2
     * @param n Número de entrada
     * @return La potencia de 2 más cercana (mayor o igual)
     */
    int next_power_of_2(int n);

    /**
     * @brief Genera una lista de números de procesos para benchmark
     *
     * Genera: [1, 2, 4, 8, ..., max_processes]
     * Si max_processes no es potencia de 2, lo incluye al final
     *
     * @param max_processes Número máximo de procesos
     * @return Vector con números de procesos a probar
     */
    std::vector<int> generate_process_list(int max_processes);

    /**
     * @brief Calcula GFLOPS para multiplicación de matrices
     *
     * Operaciones totales: 2*N^3 (N^3 multiplicaciones + N^3 sumas)
     * GFLOPS = (2*N^3) / (time_seconds * 1e9)
     *
     * @param matrix_size Tamaño de la matriz (N x N)
     * @param time_seconds Tiempo de ejecución en segundos
     * @return GFLOPS calculados
     */
    double calculate_gflops(int matrix_size, double time_seconds);

    /**
     * @brief Convierte bytes a formato legible (KB, MB, GB)
     * @param bytes Tamaño en bytes
     * @return String formateado
     */
    std::string format_bytes(size_t bytes);

    /**
     * @brief Limpia el buffer de entrada (para evitar problemas con scanf)
     */
    void clear_input_buffer();

    /**
     * @brief Pausa la ejecución esperando que el usuario presione Enter
     */
    void pause();
}

#endif // UTILS_H
