#include "PerformanceAnalyzer.h"
#include "Utils.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>

PerformanceAnalyzer::PerformanceAnalyzer()
    : sequential_time(0.0),
      f_theoretical(Config::THEORETICAL_F),
      f_empirical(Config::THEORETICAL_F),
      f_empirical_calculated(false) {
}

void PerformanceAnalyzer::set_sequential_time(double time_seconds) {
    sequential_time = time_seconds;
}

double PerformanceAnalyzer::calculate_amdahl_speedup(int num_processes, double f) const {
    /**
     * Ley de Amdahl:
     * S(P) = 1 / ((1-f) + f/P)
     *
     * Donde:
     * - S(P) = Speedup con P procesos
     * - f = Fracción paralelizable (0.0 a 1.0)
     * - (1-f) = Fracción secuencial (cuello de botella)
     * - P = Número de procesos
     */

    if (num_processes <= 0) return 1.0;
    if (f < 0.0) f = 0.0;
    if (f > 1.0) f = 1.0;

    double sequential_fraction = 1.0 - f;
    double parallel_fraction = f / static_cast<double>(num_processes);

    return 1.0 / (sequential_fraction + parallel_fraction);
}

void PerformanceAnalyzer::calculate_empirical_f(double speedup_with_2_processes) {
    /**
     * Cálculo empírico de la fracción paralelizable.
     *
     * Dado: S(2) medido experimentalmente
     * Sabemos: S(2) = 1 / ((1-f) + f/2)
     *
     * Despejando f:
     * S(2) * ((1-f) + f/2) = 1
     * S(2) - S(2)*f + S(2)*f/2 = 1
     * S(2) - S(2)*f/2 = 1
     * S(2) - 1 = S(2)*f/2
     * f = 2 * (S(2) - 1) / S(2)
     * f = 2 * (1 - 1/S(2))
     */

    if (speedup_with_2_processes <= 1.0) {
        // Si el speedup es <= 1, algo está mal
        f_empirical = f_theoretical;
        return;
    }

    f_empirical = 2.0 * (1.0 - 1.0 / speedup_with_2_processes);

    // Limitar entre 0 y 1
    if (f_empirical < 0.0) f_empirical = 0.0;
    if (f_empirical > 1.0) f_empirical = 1.0;

    f_empirical_calculated = true;
}

std::string PerformanceAnalyzer::determine_granularity(int num_processes, double efficiency) const {
    /**
     * Granularidad basada en la eficiencia:
     *
     * Eficiencia = Speedup / P
     *
     * - Gruesa: Eficiencia > 0.8 (poco overhead, mucho cómputo)
     * - Media: 0.5 < Eficiencia <= 0.8
     * - Fina: Eficiencia <= 0.5 (mucho overhead)
     */

    if (num_processes == 1) {
        return "N/A";
    }

    if (efficiency > 0.8) {
        return "Gruesa";
    } else if (efficiency > 0.5) {
        return "Media";
    } else {
        return "Fina";
    }
}

void PerformanceAnalyzer::analyze_result(int num_processes, double time_seconds, int matrix_size) {
    BenchmarkResult result;

    result.num_processes = num_processes;
    result.time_seconds = time_seconds;
    result.gflops = Utils::calculate_gflops(matrix_size, time_seconds);

    // Calcular speedup experimental
    if (sequential_time > 0.0) {
        result.speedup_exp = sequential_time / time_seconds;
    } else {
        result.speedup_exp = 1.0;
    }

    // Si este es el resultado con P=2, calcular f empírico
    if (num_processes == 2 && !f_empirical_calculated) {
        calculate_empirical_f(result.speedup_exp);
    }

    // Calcular speedup teórico según Amdahl (usar f empírico si está disponible)
    double f_to_use = f_empirical_calculated ? f_empirical : f_theoretical;
    result.speedup_amdahl = calculate_amdahl_speedup(num_processes, f_to_use);

    // Calcular eficiencia
    result.efficiency = result.speedup_exp / static_cast<double>(num_processes);

    // Determinar granularidad
    result.granularity = determine_granularity(num_processes, result.efficiency);

    results.push_back(result);
}

double PerformanceAnalyzer::get_empirical_f() const {
    return f_empirical;
}

double PerformanceAnalyzer::get_theoretical_f() const {
    return f_theoretical;
}

double PerformanceAnalyzer::get_max_theoretical_speedup() const {
    /**
     * Speedup máximo con infinitos procesos:
     * S_max = lim(P->∞) S(P) = 1 / (1-f)
     */
    double f_to_use = f_empirical_calculated ? f_empirical : f_theoretical;
    return 1.0 / (1.0 - f_to_use);
}

const std::vector<BenchmarkResult>& PerformanceAnalyzer::get_results() const {
    return results;
}

void PerformanceAnalyzer::clear_results() {
    results.clear();
    f_empirical_calculated = false;
    sequential_time = 0.0;
}

void PerformanceAnalyzer::print_sequential_table(const BenchmarkResult& result) const {
    Utils::print_header("RESULTADOS SECUENCIAL (P=1)");

    std::cout << std::left;
    std::cout << "| " << std::setw(20) << "Metrica" << " | " << std::setw(15) << "Valor" << " |" << std::endl;
    Utils::print_separator(40, '-');
    std::cout << "| " << std::setw(20) << "Tiempo (s)" << " | "
              << std::setw(15) << std::fixed << std::setprecision(6) << result.time_seconds << " |" << std::endl;
    std::cout << "| " << std::setw(20) << "GFLOPS" << " | "
              << std::setw(15) << std::fixed << std::setprecision(3) << result.gflops << " |" << std::endl;

    Utils::print_separator();
}

void PerformanceAnalyzer::print_parallel_table(const BenchmarkResult& result) const {
    Utils::print_header("RESULTADOS PARALELO");

    std::cout << std::left;
    std::cout << "| " << std::setw(20) << "Metrica" << " | " << std::setw(15) << "Valor" << " |" << std::endl;
    Utils::print_separator(40, '-');
    std::cout << "| " << std::setw(20) << "Procesos" << " | "
              << std::setw(15) << result.num_processes << " |" << std::endl;
    std::cout << "| " << std::setw(20) << "Tiempo (s)" << " | "
              << std::setw(15) << std::fixed << std::setprecision(6) << result.time_seconds << " |" << std::endl;
    std::cout << "| " << std::setw(20) << "GFLOPS" << " | "
              << std::setw(15) << std::fixed << std::setprecision(3) << result.gflops << " |" << std::endl;
    std::cout << "| " << std::setw(20) << "Speedup Exp" << " | "
              << std::setw(15) << std::fixed << std::setprecision(2) << result.speedup_exp << "x |" << std::endl;

    if (sequential_time > 0.0) {
        std::cout << "| " << std::setw(20) << "Tiempo Secuencial" << " | "
                  << std::setw(15) << std::fixed << std::setprecision(6) << sequential_time << " |" << std::endl;
    }

    Utils::print_separator();
}

void PerformanceAnalyzer::print_benchmark_table() const {
    if (results.empty()) {
        std::cout << "No hay resultados para mostrar." << std::endl;
        return;
    }

    // 1. Tabla Secuencial (P=1)
    if (!results.empty() && results[0].num_processes == 1) {
        print_sequential_table(results[0]);
        std::cout << std::endl;
    }

    // 2. Tabla Paralela
    Utils::print_header("TABLA PARALELA - COMPARACION");

    // Encabezado
    std::cout << std::left;
    std::cout << "| " << std::setw(10) << "Procesos"
              << " | " << std::setw(12) << "Tiempo (s)"
              << " | " << std::setw(12) << "Speedup Exp"
              << " | " << std::setw(14) << "Speedup Amdahl"
              << " | " << std::setw(12) << "Eficiencia"
              << " | " << std::setw(12) << "Granularidad"
              << " |" << std::endl;
    Utils::print_separator(90, '-');

    // Datos
    for (const auto& result : results) {
        std::cout << "| " << std::setw(10) << result.num_processes
                  << " | " << std::setw(12) << std::fixed << std::setprecision(6) << result.time_seconds
                  << " | " << std::setw(12) << std::fixed << std::setprecision(2) << result.speedup_exp << "x"
                  << " | " << std::setw(14) << std::fixed << std::setprecision(2) << result.speedup_amdahl << "x"
                  << " | " << std::setw(12) << std::fixed << std::setprecision(1) << (result.efficiency * 100) << "%"
                  << " | " << std::setw(12) << result.granularity
                  << " |" << std::endl;
    }

    Utils::print_separator();
    std::cout << std::endl;

    // 3. Análisis de fracción paralelizable
    print_f_analysis();

    // 4. Análisis final
    if (results.size() > 1) {
        Utils::print_header("ANALISIS FINAL");

        const auto& best = results.back();

        std::cout << std::left;
        std::cout << std::setw(35) << "Mejor speedup observado:"
                  << std::fixed << std::setprecision(2) << best.speedup_exp << "x con "
                  << best.num_processes << " procesos" << std::endl;

        double max_speedup = get_max_theoretical_speedup();
        std::cout << std::setw(35) << "Speedup maximo teorico (Amdahl):"
                  << std::fixed << std::setprecision(2) << max_speedup << "x (P->infinito)" << std::endl;

        double f_to_use = f_empirical_calculated ? f_empirical : f_theoretical;
        double limiting_factor = (1.0 - f_to_use) * 100.0;
        std::cout << std::setw(35) << "Limitacion por codigo secuencial:"
                  << std::fixed << std::setprecision(1) << limiting_factor << "%" << std::endl;

        Utils::print_separator();
    }
}

void PerformanceAnalyzer::print_f_analysis() const {
    Utils::print_header("FRACCION PARALELIZABLE (f)");

    std::cout << std::left;
    std::cout << std::setw(35) << "f (teorico asumido):"
              << std::fixed << std::setprecision(3) << f_theoretical
              << " (" << (f_theoretical * 100) << "% paralelizable)" << std::endl;

    if (f_empirical_calculated) {
        std::cout << std::setw(35) << "f (empirico medido):"
                  << std::fixed << std::setprecision(3) << f_empirical
                  << " (" << (f_empirical * 100) << "% paralelizable)" << std::endl;
        std::cout << std::setw(35) << "Metodo usado:"
                  << "Calculado de S(2) experimental" << std::endl;
    } else {
        std::cout << std::setw(35) << "f (empirico):"
                  << "No calculado aun (requiere P=2)" << std::endl;
    }

    Utils::print_separator();
    std::cout << std::endl;
}

bool PerformanceAnalyzer::export_to_csv(const std::string& filename) const {
    /**
     * Exporta los resultados del benchmark a un archivo CSV
     * para poder ser graficados con Python/matplotlib
     */

    if (results.empty()) {
        std::cerr << "Error: No hay resultados para exportar." << std::endl;
        return false;
    }

    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo " << filename << " para escritura." << std::endl;
        return false;
    }

    // Escribir encabezado CSV
    file << "Procesos,Tiempo(s),Speedup_Exp,Speedup_Amdahl,Eficiencia,GFLOPS,Granularidad\n";

    // Escribir datos
    for (const auto& result : results) {
        file << result.num_processes << ","
             << std::fixed << std::setprecision(6) << result.time_seconds << ","
             << std::fixed << std::setprecision(4) << result.speedup_exp << ","
             << std::fixed << std::setprecision(4) << result.speedup_amdahl << ","
             << std::fixed << std::setprecision(4) << result.efficiency << ","
             << std::fixed << std::setprecision(3) << result.gflops << ","
             << result.granularity << "\n";
    }

    file.close();

    std::cout << "\n";
    Utils::print_separator(60, '=');
    std::cout << "Resultados exportados exitosamente a: " << filename << std::endl;
    std::cout << "Para ver las graficas, ejecuta:\n";
    std::cout << "  python3 plot_results.py\n";
    Utils::print_separator(60, '=');
    std::cout << "\n";

    return true;
}
