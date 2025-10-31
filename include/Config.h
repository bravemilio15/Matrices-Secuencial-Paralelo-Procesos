#ifndef CONFIG_H
#define CONFIG_H

#include <string>

// Constantes de configuración por defecto
namespace Config {
    // Tamaños por defecto de matrices
    const int DEFAULT_MATRIX_SIZE = 500;

    // Tamaño de bloque por defecto (se auto-ajustará según caché)
    const int MIN_BLOCK_SIZE = 32;
    const int MAX_BLOCK_SIZE = 256;
    const int DEFAULT_BLOCK_SIZE = 64;

    // Constantes para cálculos
    const double GIGA = 1e9;
    const double MEGA = 1e6;

    // Fracción paralelizable teórica asumida inicialmente
    const double THEORETICAL_F = 0.98;

    // Número mínimo de procesos para calcular f empírico
    const int MIN_PROCESSES_FOR_EMPIRICAL_F = 2;
}

// Estructura para almacenar información del hardware
struct HardwareInfo {
    int logical_cores;      // Núcleos lógicos disponibles
    int physical_cores;     // Núcleos físicos (estimado)
    size_t l1_cache_size;   // Tamaño de caché L1 en bytes
    size_t l2_cache_size;   // Tamaño de caché L2 en bytes
    size_t l3_cache_size;   // Tamaño de caché L3 en bytes
    std::string os_name;    // Sistema operativo
    std::string cpu_model;  // Modelo de CPU

    HardwareInfo()
        : logical_cores(1), physical_cores(1),
          l1_cache_size(32768), l2_cache_size(256*1024), l3_cache_size(8*1024*1024),
          os_name("Unknown"), cpu_model("Unknown") {}
};

// Estructura para almacenar configuración actual
struct ProgramConfig {
    int matrix_size;        // Tamaño de la matriz (N x N)
    int block_size;         // Tamaño del bloque para multiplicación
    int max_processes;      // Máximo número de procesos (del hardware)
    bool auto_block_size;   // Si el tamaño de bloque es automático

    ProgramConfig()
        : matrix_size(Config::DEFAULT_MATRIX_SIZE),
          block_size(Config::DEFAULT_BLOCK_SIZE),
          max_processes(1),
          auto_block_size(true) {}
};

// Estructura para almacenar resultados de una ejecución
struct BenchmarkResult {
    int num_processes;      // Número de procesos usados
    double time_seconds;    // Tiempo de ejecución en segundos
    double gflops;          // Operaciones de punto flotante por segundo
    double speedup_exp;     // Speedup experimental
    double speedup_amdahl;  // Speedup teórico según Amdahl
    double efficiency;      // Eficiencia (speedup/num_processes)
    std::string granularity; // Tipo de granularidad (Gruesa/Media/Fina)

    BenchmarkResult()
        : num_processes(1), time_seconds(0.0), gflops(0.0),
          speedup_exp(1.0), speedup_amdahl(1.0), efficiency(1.0),
          granularity("N/A") {}
};

// Enumeración para tipos de ejecución
enum class ExecutionType {
    SEQUENTIAL,
    PARALLEL,
    BENCHMARK
};

// Enumeración para tipo de granularidad
enum class GranularityType {
    FINE,       // Fina: mucha comunicación, poco cómputo
    MEDIUM,     // Media: balance
    COARSE,     // Gruesa: poca comunicación, mucho cómputo
    NA          // No aplicable
};

#endif // CONFIG_H
