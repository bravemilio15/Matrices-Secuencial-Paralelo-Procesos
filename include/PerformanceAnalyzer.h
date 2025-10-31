#ifndef PERFORMANCE_ANALYZER_H
#define PERFORMANCE_ANALYZER_H

#include "Config.h"
#include <vector>

/**
 * @brief Clase para analizar rendimiento y aplicar Ley de Amdahl
 *
 * Calcula:
 * - Speedup experimental y teórico
 * - Fracción paralelizable (f) empíricamente
 * - Eficiencia
 * - Granularidad
 * - Genera tablas de resultados
 */
class PerformanceAnalyzer {
private:
    double sequential_time;     // Tiempo de referencia secuencial
    double f_theoretical;       // Fracción paralelizable teórica (0.98)
    double f_empirical;         // Fracción paralelizable calculada empíricamente
    bool f_empirical_calculated; // Si ya se calculó f empírico

    std::vector<BenchmarkResult> results; // Resultados acumulados

    /**
     * @brief Calcula el speedup teórico de Amdahl
     *
     * Fórmula: S(P) = 1 / ((1-f) + f/P)
     *
     * @param num_processes Número de procesos
     * @param f Fracción paralelizable
     * @return Speedup teórico
     */
    double calculate_amdahl_speedup(int num_processes, double f) const;

    /**
     * @brief Determina el tipo de granularidad según overhead
     *
     * Granularidad = Tiempo_Computación / Tiempo_Comunicación
     *
     * - Gruesa (Coarse): ratio > 10 (poco overhead)
     * - Media (Medium): 2 < ratio <= 10
     * - Fina (Fine): ratio <= 2 (mucho overhead)
     *
     * @param num_processes Número de procesos
     * @param efficiency Eficiencia observada
     * @return Tipo de granularidad
     */
    std::string determine_granularity(int num_processes, double efficiency) const;

public:
    /**
     * @brief Constructor
     */
    PerformanceAnalyzer();

    /**
     * @brief Establece el tiempo secuencial de referencia (P=1)
     * @param time_seconds Tiempo en segundos
     */
    void set_sequential_time(double time_seconds);

    /**
     * @brief Calcula la fracción paralelizable empíricamente
     *
     * Usa la medición con P=2 para despejar f:
     * S(2) = 1 / ((1-f) + f/2)
     * Despejando: f = 2 * (1 - 1/S(2))
     *
     * @param speedup_with_2_processes Speedup medido con 2 procesos
     */
    void calculate_empirical_f(double speedup_with_2_processes);

    /**
     * @brief Analiza un resultado de ejecución
     *
     * Calcula todas las métricas y las almacena
     *
     * @param num_processes Número de procesos usado
     * @param time_seconds Tiempo de ejecución
     * @param matrix_size Tamaño de la matriz
     */
    void analyze_result(int num_processes, double time_seconds, int matrix_size);

    /**
     * @brief Obtiene la fracción paralelizable empírica
     * @return f empírico (o teórico si aún no se calculó)
     */
    double get_empirical_f() const;

    /**
     * @brief Obtiene la fracción paralelizable teórica
     * @return f teórico (0.98)
     */
    double get_theoretical_f() const;

    /**
     * @brief Calcula el speedup máximo teórico con infinitos procesos
     *
     * S_max = 1 / (1 - f)
     *
     * @return Speedup máximo posible
     */
    double get_max_theoretical_speedup() const;

    /**
     * @brief Obtiene todos los resultados acumulados
     * @return Vector de resultados
     */
    const std::vector<BenchmarkResult>& get_results() const;

    /**
     * @brief Limpia todos los resultados acumulados
     */
    void clear_results();

    /**
     * @brief Imprime tabla de resultados para ejecución secuencial
     * @param result Resultado a mostrar
     */
    void print_sequential_table(const BenchmarkResult& result) const;

    /**
     * @brief Imprime tabla de resultados para ejecución paralela
     * @param result Resultado a mostrar
     */
    void print_parallel_table(const BenchmarkResult& result) const;

    /**
     * @brief Imprime tabla completa de benchmark con todos los resultados
     *
     * Muestra:
     * - Tabla secuencial (P=1)
     * - Tabla paralela (P=2, 4, 8, ...)
     * - Análisis de fracción paralelizable
     * - Limitaciones según Amdahl
     */
    void print_benchmark_table() const;

    /**
     * @brief Imprime análisis de la fracción paralelizable
     */
    void print_f_analysis() const;

    /**
     * @brief Exporta resultados a archivo CSV para graficación
     *
     * Genera un archivo CSV con formato:
     * Procesos,Tiempo(s),Speedup_Exp,Speedup_Amdahl,Eficiencia,GFLOPS,Granularidad
     *
     * @param filename Nombre del archivo CSV a generar
     * @return true si se exportó correctamente, false en caso de error
     */
    bool export_to_csv(const std::string& filename) const;
};

#endif // PERFORMANCE_ANALYZER_H
