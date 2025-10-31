#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#include <string>
#include <cstddef>

/**
 * @brief Clase para gestionar memoria compartida entre procesos (IPC)
 *
 * Implementa patrón RAII:
 * - Constructor: crea/abre memoria compartida
 * - Destructor: libera memoria compartida
 *
 * Compatible con:
 * - Linux/macOS: shm_open() + mmap()
 * - Windows: CreateFileMapping() + MapViewOfFile()
 */
class SharedMemory {
private:
    std::string name;           // Nombre de la memoria compartida
    size_t size;                // Tamaño en bytes
    void* ptr;                  // Puntero a la memoria compartida
    bool is_owner;              // Si este objeto creó la memoria (para cleanup)

    #ifdef _WIN32
        void* handle;           // Handle de Windows
    #else
        int fd;                 // File descriptor (POSIX)
    #endif

    bool created;               // Si la memoria fue creada exitosamente

public:
    /**
     * @brief Constructor: crea memoria compartida
     *
     * @param name Nombre único para la memoria compartida
     * @param size Tamaño en bytes
     * @param create Si true, crea nueva memoria. Si false, abre existente
     */
    SharedMemory(const std::string& name, size_t size, bool create = true);

    /**
     * @brief Destructor: libera la memoria compartida
     */
    ~SharedMemory();

    // Deshabilitar copia para evitar doble liberación
    SharedMemory(const SharedMemory&) = delete;
    SharedMemory& operator=(const SharedMemory&) = delete;

    /**
     * @brief Obtiene puntero a la memoria compartida
     * @return Puntero void* que puede ser casteado al tipo deseado
     */
    void* get_ptr();

    /**
     * @brief Obtiene puntero const a la memoria compartida
     * @return Puntero const void*
     */
    const void* get_ptr() const;

    /**
     * @brief Verifica si la memoria fue creada/abierta exitosamente
     * @return true si está disponible
     */
    bool is_valid() const;

    /**
     * @brief Obtiene el tamaño de la memoria compartida
     * @return Tamaño en bytes
     */
    size_t get_size() const;

    /**
     * @brief Libera explícitamente la memoria compartida
     * Llamado automáticamente en el destructor
     */
    void release();

    /**
     * @brief Crea memoria compartida para matriz
     *
     * Helper estático para calcular el tamaño necesario
     *
     * @param rows Número de filas
     * @param cols Número de columnas
     * @return Tamaño en bytes necesario
     */
    static size_t matrix_size(int rows, int cols);
};

#endif // SHARED_MEMORY_H
