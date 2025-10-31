#ifndef HARDWARE_DETECTOR_H
#define HARDWARE_DETECTOR_H

#include "Config.h"
#include <string>

/**
 * @brief Clase para detectar características del hardware
 *
 * Detecta:
 * - Número de núcleos lógicos y físicos
 * - Tamaño de cachés (L1, L2, L3)
 * - Sistema operativo
 * - Modelo de CPU
 *
 * Compatible con Linux, macOS y Windows
 */
class HardwareDetector {
private:
    HardwareInfo info;

    /**
     * @brief Detecta el número de núcleos lógicos
     * Usa std::thread::hardware_concurrency() como base
     */
    void detect_cores();

    /**
     * @brief Detecta tamaños de caché según el sistema operativo
     *
     * Linux: Lee /sys/devices/system/cpu/cpu0/cache/
     * macOS: Usa sysctlbyname()
     * Windows: Usa GetLogicalProcessorInformation()
     */
    void detect_cache_sizes();

    /**
     * @brief Detecta el sistema operativo
     */
    void detect_os();

    /**
     * @brief Detecta el modelo de CPU
     *
     * Linux: Lee /proc/cpuinfo
     * macOS: Usa sysctlbyname()
     * Windows: Usa registro
     */
    void detect_cpu_model();

public:
    HardwareDetector();

    /**
     * @brief Ejecuta toda la detección de hardware
     */
    void detect();

    /**
     * @brief Obtiene la información detectada
     * @return Estructura HardwareInfo con toda la información
     */
    HardwareInfo get_info() const;

    /**
     * @brief Calcula el tamaño óptimo de bloque basado en caché L1
     *
     * Fórmula: bloque = sqrt(L1_cache / (3 * sizeof(double)))
     * Razón: 3 matrices (A, B, C) deben caber en caché simultáneamente
     *
     * @return Tamaño de bloque óptimo (potencia de 2 entre 32 y 256)
     */
    int calculate_optimal_block_size() const;

    /**
     * @brief Imprime la información del hardware en formato tabla
     */
    void print_info() const;
};

#endif // HARDWARE_DETECTOR_H
