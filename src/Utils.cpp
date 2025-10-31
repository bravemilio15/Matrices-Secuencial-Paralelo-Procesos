#include "Utils.h"
#include "Config.h"
#include <iostream>
#include <iomanip>
#include <cmath>
#include <limits>

// Implementación de Timer
Timer::Timer() : is_running(false) {}

void Timer::start() {
    start_time = std::chrono::high_resolution_clock::now();
    is_running = true;
}

void Timer::stop() {
    end_time = std::chrono::high_resolution_clock::now();
    is_running = false;
}

double Timer::elapsed_seconds() const {
    auto end = is_running ? std::chrono::high_resolution_clock::now() : end_time;
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start_time);
    return duration.count() / 1e6;
}

double Timer::elapsed_milliseconds() const {
    return elapsed_seconds() * 1000.0;
}

// Implementación de funciones de utilidad
namespace Utils {

    void print_separator(int length, char character) {
        std::cout << std::string(length, character) << std::endl;
    }

    void print_header(const std::string& title, int width) {
        print_separator(width);
        int padding = (width - title.length()) / 2;
        std::cout << std::string(padding, ' ') << title << std::endl;
        print_separator(width);
    }

    int next_power_of_2(int n) {
        if (n <= 0) return 1;

        // Si ya es potencia de 2, retornar el mismo número
        if ((n & (n - 1)) == 0) return n;

        // Encontrar la siguiente potencia de 2
        int power = 1;
        while (power < n) {
            power *= 2;
        }
        return power;
    }

    std::vector<int> generate_process_list(int max_processes) {
        std::vector<int> list;

        // Siempre empezar con 1 (secuencial)
        list.push_back(1);

        // Agregar potencias de 2: 2, 4, 8, 16, ...
        int p = 2;
        while (p <= max_processes) {
            list.push_back(p);
            p *= 2;
        }

        // Si max_processes no está en la lista, agregarlo
        if (list.back() != max_processes) {
            list.push_back(max_processes);
        }

        return list;
    }

    double calculate_gflops(int matrix_size, double time_seconds) {
        if (time_seconds <= 0.0) return 0.0;

        // Operaciones de punto flotante para multiplicación de matrices:
        // Para cada elemento de C[i][j]: N multiplicaciones + N sumas = 2N operaciones
        // Total de elementos en C: N x N
        // Total de operaciones: 2 * N^3
        double operations = 2.0 * matrix_size * matrix_size * matrix_size;

        // GFLOPS = operaciones / (tiempo_segundos * 1e9)
        return operations / (time_seconds * Config::GIGA);
    }

    std::string format_bytes(size_t bytes) {
        const char* units[] = {"B", "KB", "MB", "GB", "TB"};
        int unit_index = 0;
        double size = static_cast<double>(bytes);

        while (size >= 1024.0 && unit_index < 4) {
            size /= 1024.0;
            unit_index++;
        }

        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2) << size << " " << units[unit_index];
        return oss.str();
    }

    void clear_input_buffer() {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    void pause() {
        std::cout << "\nPresione Enter para continuar...";
        clear_input_buffer();
        std::cin.get();
    }

}
