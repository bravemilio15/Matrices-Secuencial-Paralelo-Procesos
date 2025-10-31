#include "HardwareDetector.h"
#include "Utils.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <cmath>
#include <iomanip>
#include <set>

// Headers específicos de plataforma
#ifdef _WIN32
    #include <windows.h>
#elif __APPLE__
    #include <sys/types.h>
    #include <sys/sysctl.h>
#elif __linux__
    #include <unistd.h>
#endif

HardwareDetector::HardwareDetector() {
    // Constructor vacío, la detección se hace en detect()
}

void HardwareDetector::detect_cores() {
    // Detectar núcleos lógicos usando std::thread
    info.logical_cores = std::thread::hardware_concurrency();

    if (info.logical_cores == 0) {
        info.logical_cores = 1; // Fallback si falla la detección
    }

    // Estimar núcleos físicos (aproximación: lógicos / 2 si hay hyperthreading)
    #ifdef __linux__
        // En Linux, intentar leer de /proc/cpuinfo
        std::ifstream cpuinfo("/proc/cpuinfo");
        if (cpuinfo.is_open()) {
            std::string line;
            int physical_id = -1;
            std::set<int> physical_ids;

            while (std::getline(cpuinfo, line)) {
                if (line.find("physical id") == 0) {
                    size_t pos = line.find(':');
                    if (pos != std::string::npos) {
                        physical_id = std::stoi(line.substr(pos + 1));
                        physical_ids.insert(physical_id);
                    }
                }
            }

            if (!physical_ids.empty()) {
                info.physical_cores = info.logical_cores / physical_ids.size();
            } else {
                info.physical_cores = info.logical_cores;
            }
        } else {
            info.physical_cores = info.logical_cores;
        }
    #elif __APPLE__
        // En macOS, usar sysctlbyname
        int physical = 0;
        size_t size = sizeof(physical);
        if (sysctlbyname("hw.physicalcpu", &physical, &size, NULL, 0) == 0) {
            info.physical_cores = physical;
        } else {
            info.physical_cores = info.logical_cores;
        }
    #else
        // Fallback: asumir que físicos = lógicos
        info.physical_cores = info.logical_cores;
    #endif
}

void HardwareDetector::detect_cache_sizes() {
    // Valores por defecto (típicos)
    info.l1_cache_size = 32 * 1024;       // 32 KB
    info.l2_cache_size = 256 * 1024;      // 256 KB
    info.l3_cache_size = 8 * 1024 * 1024; // 8 MB

    #ifdef __linux__
        // En Linux, leer desde sysfs
        std::ifstream l1_file("/sys/devices/system/cpu/cpu0/cache/index0/size");
        if (l1_file.is_open()) {
            std::string size_str;
            l1_file >> size_str;
            // El formato es "32K" o "256K"
            int value = std::stoi(size_str);
            if (size_str.back() == 'K') {
                info.l1_cache_size = value * 1024;
            }
        }

        std::ifstream l2_file("/sys/devices/system/cpu/cpu0/cache/index2/size");
        if (l2_file.is_open()) {
            std::string size_str;
            l2_file >> size_str;
            int value = std::stoi(size_str);
            if (size_str.back() == 'K') {
                info.l2_cache_size = value * 1024;
            } else if (size_str.back() == 'M') {
                info.l2_cache_size = value * 1024 * 1024;
            }
        }

        std::ifstream l3_file("/sys/devices/system/cpu/cpu0/cache/index3/size");
        if (l3_file.is_open()) {
            std::string size_str;
            l3_file >> size_str;
            int value = std::stoi(size_str);
            if (size_str.back() == 'M') {
                info.l3_cache_size = value * 1024 * 1024;
            }
        }
    #elif __APPLE__
        // En macOS, usar sysctlbyname
        size_t size = sizeof(size_t);
        sysctlbyname("hw.l1dcachesize", &info.l1_cache_size, &size, NULL, 0);
        sysctlbyname("hw.l2cachesize", &info.l2_cache_size, &size, NULL, 0);
        sysctlbyname("hw.l3cachesize", &info.l3_cache_size, &size, NULL, 0);
    #endif
    // Windows: usar GetLogicalProcessorInformation (implementación más compleja, omitida por brevedad)
}

void HardwareDetector::detect_os() {
    #ifdef _WIN32
        info.os_name = "Windows";
    #elif __APPLE__
        info.os_name = "macOS";
    #elif __linux__
        info.os_name = "Linux";
    #else
        info.os_name = "Unknown";
    #endif
}

void HardwareDetector::detect_cpu_model() {
    #ifdef __linux__
        std::ifstream cpuinfo("/proc/cpuinfo");
        if (cpuinfo.is_open()) {
            std::string line;
            while (std::getline(cpuinfo, line)) {
                if (line.find("model name") == 0) {
                    size_t pos = line.find(':');
                    if (pos != std::string::npos) {
                        info.cpu_model = line.substr(pos + 2);
                        break;
                    }
                }
            }
        }
    #elif __APPLE__
        char cpu_brand[256];
        size_t size = sizeof(cpu_brand);
        if (sysctlbyname("machdep.cpu.brand_string", &cpu_brand, &size, NULL, 0) == 0) {
            info.cpu_model = std::string(cpu_brand);
        }
    #else
        info.cpu_model = "Unknown CPU";
    #endif
}

void HardwareDetector::detect() {
    detect_cores();
    detect_cache_sizes();
    detect_os();
    detect_cpu_model();
}

HardwareInfo HardwareDetector::get_info() const {
    return info;
}

int HardwareDetector::calculate_optimal_block_size() const {
    // Calcular tamaño de bloque óptimo basado en caché L1
    // Necesitamos que 3 bloques (de A, B y C) quepan en caché
    // Cada bloque tiene block_size^2 elementos de tipo double (8 bytes)

    // Fórmula: block_size = sqrt(L1_cache / (3 * sizeof(double)))
    double elements_per_cache = info.l1_cache_size / (3.0 * sizeof(double));
    int block_size = static_cast<int>(std::sqrt(elements_per_cache));

    // Ajustar a la potencia de 2 más cercana
    block_size = Utils::next_power_of_2(block_size);

    // Limitar entre MIN y MAX
    if (block_size < Config::MIN_BLOCK_SIZE) {
        block_size = Config::MIN_BLOCK_SIZE;
    }
    if (block_size > Config::MAX_BLOCK_SIZE) {
        block_size = Config::MAX_BLOCK_SIZE;
    }

    return block_size;
}

void HardwareDetector::print_info() const {
    Utils::print_header("INFORMACION DEL HARDWARE");

    std::cout << std::left;
    std::cout << std::setw(30) << "Sistema Operativo:" << info.os_name << std::endl;
    std::cout << std::setw(30) << "Modelo de CPU:" << info.cpu_model << std::endl;
    std::cout << std::setw(30) << "Nucleos Logicos:" << info.logical_cores << std::endl;
    std::cout << std::setw(30) << "Nucleos Fisicos (est.):" << info.physical_cores << std::endl;
    std::cout << std::setw(30) << "Cache L1:" << Utils::format_bytes(info.l1_cache_size) << std::endl;
    std::cout << std::setw(30) << "Cache L2:" << Utils::format_bytes(info.l2_cache_size) << std::endl;
    std::cout << std::setw(30) << "Cache L3:" << Utils::format_bytes(info.l3_cache_size) << std::endl;

    int optimal_block = calculate_optimal_block_size();
    std::cout << std::setw(30) << "Tamanio de bloque optimo:" << optimal_block << "x" << optimal_block << std::endl;

    Utils::print_separator();
}
