cmake_minimum_required(VERSION 3.10)
project(MatrixMultiplication VERSION 1.0 LANGUAGES CXX)

# Establecer estándar de C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Flags de compilación
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")

# Directorios de include
include_directories(${PROJECT_SOURCE_DIR}/include)

# Archivos fuente
set(SOURCES
    src/main.cpp
    src/HardwareDetector.cpp
    src/MatrixMultiplier.cpp
    src/ProcessManager.cpp
    src/PerformanceAnalyzer.cpp
    src/SharedMemory.cpp
    src/Utils.cpp
)

# Crear ejecutable
add_executable(matrix_mult ${SOURCES})

# Enlazar bibliotecas según la plataforma
if(UNIX AND NOT APPLE)
    # Linux
    target_link_libraries(matrix_mult pthread rt)
elseif(APPLE)
    # macOS
    target_link_libraries(matrix_mult pthread)
elseif(WIN32)
    # Windows
    # No necesita bibliotecas adicionales
endif()

# Información de compilación
message(STATUS "Sistema: ${CMAKE_SYSTEM_NAME}")
message(STATUS "Compilador: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "Estándar C++: ${CMAKE_CXX_STANDARD}")

# Opciones de instalación (opcional)
install(TARGETS matrix_mult DESTINATION bin)

# Target para limpiar
add_custom_target(clean-all
    COMMAND ${CMAKE_BUILD_TOOL} clean
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/CMakeFiles
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/CMakeCache.txt
    COMMENT "Limpiando todos los archivos generados..."
)
