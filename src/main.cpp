#include "Config.h"
#include "HardwareDetector.h"
#include "MatrixMultiplier.h"
#include "ProcessManager.h"
#include "PerformanceAnalyzer.h"
#include "Utils.h"
#include <iostream>
#include <iomanip>
#include <memory>

/**
 * @brief Clase principal para gestionar el programa
 */
class MatrixMultiplicationApp {
private:
    ProgramConfig config;
    HardwareInfo hardware;
    HardwareDetector hw_detector;
    PerformanceAnalyzer analyzer;

    /**
     * @brief Muestra el menú principal
     */
    void show_menu() {
        Utils::print_separator(60);
        std::cout << "    MULTIPLICACION DE MATRICES EN C++\n";
        std::cout << "    Procesos + Optimizacion por Bloques\n";
        Utils::print_separator(60);
        std::cout << "1. Mostrar informacion del hardware\n";
        std::cout << "2. Ejecutar multiplicacion SECUENCIAL\n";
        std::cout << "3. Ejecutar multiplicacion PARALELA (P_max)\n";
        std::cout << "4. Ejecutar BENCHMARK COMPLETO (comparacion)\n";
        std::cout << "5. Configurar tamanio de matrices\n";
        std::cout << "6. Mostrar configuracion actual\n";
        std::cout << "7. Salir\n";
        Utils::print_separator(60);
        std::cout << "Seleccione una opcion: ";
    }

    /**
     * @brief Opción 1: Mostrar información del hardware
     */
    void option_show_hardware() {
        std::cout << "\n";
        hw_detector.print_info();
        Utils::pause();
    }

    /**
     * @brief Opción 2: Ejecutar multiplicación secuencial
     */
    void option_sequential() {
        std::cout << "\n";
        Utils::print_header("MULTIPLICACION SECUENCIAL");

        // Crear multiplicador
        MatrixMultiplier multiplier(config.matrix_size, config.block_size);

        // Mostrar información de bloques
        multiplier.print_block_info();

        // Inicializar matrices
        std::cout << "Inicializando matrices..." << std::endl;
        multiplier.initialize_matrices();

        // Medir tiempo
        std::cout << "Ejecutando multiplicacion secuencial..." << std::endl;
        Timer timer;
        timer.start();
        multiplier.multiply_sequential();
        timer.stop();

        double elapsed = timer.elapsed_seconds();

        // Analizar resultado
        analyzer.clear_results();
        analyzer.set_sequential_time(elapsed);
        analyzer.analyze_result(1, elapsed, config.matrix_size);

        // Mostrar tabla
        const auto& results = analyzer.get_results();
        if (!results.empty()) {
            analyzer.print_sequential_table(results[0]);
        }

        Utils::pause();
    }

    /**
     * @brief Opción 3: Ejecutar multiplicación paralela con P_max
     */
    void option_parallel() {
        std::cout << "\n";
        Utils::print_header("MULTIPLICACION PARALELA");

        int num_processes = config.max_processes;

        std::cout << "Procesos a usar: " << num_processes << " (maximo del hardware)\n\n";

        // Crear multiplicador para obtener datos
        MatrixMultiplier multiplier(config.matrix_size, config.block_size);
        multiplier.print_block_info();

        // Inicializar matrices
        std::cout << "Inicializando matrices..." << std::endl;
        multiplier.initialize_matrices();

        // Primero ejecutar secuencial para tener referencia
        std::cout << "Ejecutando version secuencial (referencia)..." << std::endl;
        Timer timer_seq;
        timer_seq.start();
        multiplier.multiply_sequential();
        timer_seq.stop();
        double time_seq = timer_seq.elapsed_seconds();

        // Limpiar resultado
        multiplier.clear_result();

        // Ejecutar paralelo
        std::cout << "Ejecutando multiplicacion paralela con " << num_processes << " procesos..." << std::endl;
        ProcessManager pm(config.matrix_size, config.block_size, num_processes);

        Timer timer_par;
        timer_par.start();
        pm.multiply_parallel(multiplier.get_matrix_a(), multiplier.get_matrix_b(), multiplier.get_matrix_c());
        timer_par.stop();
        double time_par = timer_par.elapsed_seconds();

        // Analizar resultados
        analyzer.clear_results();
        analyzer.set_sequential_time(time_seq);
        analyzer.analyze_result(num_processes, time_par, config.matrix_size);

        // Mostrar tabla
        const auto& results = analyzer.get_results();
        if (!results.empty()) {
            analyzer.print_parallel_table(results[0]);
        }

        Utils::pause();
    }

    /**
     * @brief Opción 4: Benchmark completo
     */
    void option_benchmark() {
        std::cout << "\n";
        Utils::print_header("BENCHMARK COMPLETO");

        // Generar lista de procesos a probar
        std::vector<int> process_list = Utils::generate_process_list(config.max_processes);

        std::cout << "Se probaran con: ";
        for (size_t i = 0; i < process_list.size(); i++) {
            std::cout << process_list[i];
            if (i < process_list.size() - 1) std::cout << ", ";
        }
        std::cout << " procesos\n\n";

        // Crear multiplicador
        MatrixMultiplier multiplier(config.matrix_size, config.block_size);
        multiplier.print_block_info();

        // Inicializar matrices
        std::cout << "Inicializando matrices..." << std::endl;
        multiplier.initialize_matrices();

        // Limpiar resultados previos
        analyzer.clear_results();

        double sequential_time = 0.0;

        // Ejecutar para cada número de procesos
        for (int num_proc : process_list) {
            std::cout << "\n--- Ejecutando con " << num_proc << " proceso(s) ---" << std::endl;

            // Limpiar resultado
            multiplier.clear_result();

            Timer timer;
            timer.start();

            if (num_proc == 1) {
                // Secuencial
                multiplier.multiply_sequential();
            } else {
                // Paralelo
                ProcessManager pm(config.matrix_size, config.block_size, num_proc);
                pm.multiply_parallel(multiplier.get_matrix_a(), multiplier.get_matrix_b(), multiplier.get_matrix_c());
            }

            timer.stop();
            double elapsed = timer.elapsed_seconds();

            std::cout << "Tiempo: " << std::fixed << std::setprecision(6) << elapsed << " segundos" << std::endl;

            // Guardar tiempo secuencial
            if (num_proc == 1) {
                sequential_time = elapsed;
                analyzer.set_sequential_time(sequential_time);
            }

            // Analizar resultado
            analyzer.analyze_result(num_proc, elapsed, config.matrix_size);
        }

        std::cout << "\n";
        // Mostrar tabla completa
        analyzer.print_benchmark_table();

        Utils::pause();
    }

    /**
     * @brief Opción 5: Configurar tamaño de matrices
     */
    void option_configure_size() {
        std::cout << "\n";
        Utils::print_header("CONFIGURAR TAMANIO DE MATRICES");

        std::cout << "Tamanio actual: " << config.matrix_size << "x" << config.matrix_size << std::endl;
        std::cout << "\nIngrese el nuevo tamanio (N para matriz NxN): ";

        int new_size;
        std::cin >> new_size;

        if (std::cin.fail() || new_size < 10 || new_size > 10000) {
            std::cout << "Tamanio invalido. Debe estar entre 10 y 10000." << std::endl;
            Utils::clear_input_buffer();
        } else {
            config.matrix_size = new_size;
            std::cout << "Tamanio actualizado a: " << config.matrix_size << "x" << config.matrix_size << std::endl;
        }

        Utils::pause();
    }

    /**
     * @brief Opción 6: Mostrar configuración actual
     */
    void option_show_config() {
        std::cout << "\n";
        Utils::print_header("CONFIGURACION ACTUAL");

        std::cout << std::left;
        std::cout << std::setw(30) << "Tamanio de matriz:" << config.matrix_size << "x" << config.matrix_size << std::endl;
        std::cout << std::setw(30) << "Tamanio de bloque:" << config.block_size << "x" << config.block_size
                  << " (auto-optimizado)" << std::endl;
        std::cout << std::setw(30) << "Procesos maximos (P_max):" << config.max_processes << std::endl;
        std::cout << std::setw(30) << "Nucleos logicos:" << hardware.logical_cores << std::endl;
        std::cout << std::setw(30) << "Nucleos fisicos:" << hardware.physical_cores << std::endl;

        Utils::print_separator();
        Utils::pause();
    }

public:
    /**
     * @brief Constructor: inicializa la aplicación
     */
    MatrixMultiplicationApp() {
        // Detectar hardware
        hw_detector.detect();
        hardware = hw_detector.get_info();

        // Configurar valores iniciales
        config.matrix_size = Config::DEFAULT_MATRIX_SIZE;
        config.block_size = hw_detector.calculate_optimal_block_size();
        config.max_processes = hardware.logical_cores;
        config.auto_block_size = true;

        // Mostrar información inicial
        std::cout << "\nPrograma de Multiplicacion de Matrices\n";
        std::cout << "Configuracion inicial:\n";
        std::cout << "- Matriz: " << config.matrix_size << "x" << config.matrix_size << std::endl;
        std::cout << "- Bloques: " << config.block_size << "x" << config.block_size << " (auto-optimizado)\n";
        std::cout << "- Procesos maximos: " << config.max_processes << std::endl;
        std::cout << std::endl;
    }

    /**
     * @brief Ejecuta el bucle principal del programa
     */
    void run() {
        int option;

        while (true) {
            show_menu();
            std::cin >> option;

            if (std::cin.fail()) {
                Utils::clear_input_buffer();
                std::cout << "\nOpcion invalida. Intente nuevamente.\n";
                Utils::pause();
                continue;
            }

            switch (option) {
                case 1:
                    option_show_hardware();
                    break;

                case 2:
                    option_sequential();
                    break;

                case 3:
                    option_parallel();
                    break;

                case 4:
                    option_benchmark();
                    break;

                case 5:
                    option_configure_size();
                    break;

                case 6:
                    option_show_config();
                    break;

                case 7:
                    std::cout << "\nSaliendo del programa...\n";
                    return;

                default:
                    std::cout << "\nOpcion invalida. Intente nuevamente.\n";
                    Utils::pause();
            }
        }
    }
};

/**
 * @brief Función principal
 */
int main() {
    try {
        MatrixMultiplicationApp app;
        app.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
