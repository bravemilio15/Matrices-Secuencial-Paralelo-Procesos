#include "SharedMemory.h"
#include <iostream>
#include <cstring>

// Headers específicos de plataforma
#ifdef _WIN32
    #include <windows.h>
#else
    #include <sys/mman.h>
    #include <sys/stat.h>
    #include <fcntl.h>
    #include <unistd.h>
#endif

SharedMemory::SharedMemory(const std::string& name, size_t size, bool create)
    : name(name), size(size), ptr(nullptr), is_owner(create), created(false) {

    #ifdef _WIN32
        // Implementación Windows
        handle = nullptr;

        if (create) {
            // Crear nueva memoria compartida
            handle = CreateFileMapping(
                INVALID_HANDLE_VALUE,
                NULL,
                PAGE_READWRITE,
                0,
                size,
                name.c_str()
            );

            if (handle == NULL) {
                std::cerr << "Error creando memoria compartida: " << GetLastError() << std::endl;
                return;
            }
        } else {
            // Abrir memoria compartida existente
            handle = OpenFileMapping(
                FILE_MAP_ALL_ACCESS,
                FALSE,
                name.c_str()
            );

            if (handle == NULL) {
                std::cerr << "Error abriendo memoria compartida: " << GetLastError() << std::endl;
                return;
            }
        }

        // Mapear la memoria
        ptr = MapViewOfFile(
            handle,
            FILE_MAP_ALL_ACCESS,
            0,
            0,
            size
        );

        if (ptr == NULL) {
            std::cerr << "Error mapeando memoria: " << GetLastError() << std::endl;
            CloseHandle(handle);
            handle = nullptr;
            return;
        }

        created = true;

    #else
        // Implementación POSIX (Linux/macOS)
        fd = -1;

        // Agregar '/' al inicio del nombre si no lo tiene (requerido en POSIX)
        std::string shm_name = name;
        if (shm_name[0] != '/') {
            shm_name = "/" + shm_name;
        }

        if (create) {
            // Crear nueva memoria compartida
            fd = shm_open(shm_name.c_str(), O_CREAT | O_RDWR, 0666);
            if (fd == -1) {
                std::cerr << "Error creando memoria compartida" << std::endl;
                return;
            }

            // Establecer el tamaño
            if (ftruncate(fd, size) == -1) {
                std::cerr << "Error estableciendo tamaño de memoria compartida" << std::endl;
                close(fd);
                shm_unlink(shm_name.c_str());
                return;
            }
        } else {
            // Abrir memoria compartida existente
            fd = shm_open(shm_name.c_str(), O_RDWR, 0666);
            if (fd == -1) {
                std::cerr << "Error abriendo memoria compartida" << std::endl;
                return;
            }
        }

        // Mapear la memoria
        ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if (ptr == MAP_FAILED) {
            std::cerr << "Error mapeando memoria compartida" << std::endl;
            close(fd);
            if (create) {
                shm_unlink(shm_name.c_str());
            }
            ptr = nullptr;
            return;
        }

        created = true;
    #endif

    // Inicializar la memoria a cero si la creamos
    if (create && ptr != nullptr) {
        std::memset(ptr, 0, size);
    }
}

SharedMemory::~SharedMemory() {
    release();
}

void* SharedMemory::get_ptr() {
    return ptr;
}

const void* SharedMemory::get_ptr() const {
    return ptr;
}

bool SharedMemory::is_valid() const {
    return created && ptr != nullptr;
}

size_t SharedMemory::get_size() const {
    return size;
}

void SharedMemory::release() {
    if (ptr == nullptr) {
        return;
    }

    #ifdef _WIN32
        // Windows
        UnmapViewOfFile(ptr);
        if (handle != nullptr) {
            CloseHandle(handle);
            handle = nullptr;
        }
    #else
        // POSIX
        munmap(ptr, size);
        if (fd != -1) {
            close(fd);
            fd = -1;
        }

        // Si somos el creador, eliminar el objeto de memoria compartida
        if (is_owner) {
            std::string shm_name = name;
            if (shm_name[0] != '/') {
                shm_name = "/" + shm_name;
            }
            shm_unlink(shm_name.c_str());
        }
    #endif

    ptr = nullptr;
    created = false;
}

size_t SharedMemory::matrix_size(int rows, int cols) {
    // Calcular espacio para matriz: rows * cols * sizeof(double)
    // Más espacio para los punteros de filas: rows * sizeof(double*)
    return rows * cols * sizeof(double) + rows * sizeof(double*);
}
