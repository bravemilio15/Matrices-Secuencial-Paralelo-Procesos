#!/bin/bash

# Script simple de compilación
# Uso: ./compile.sh

set -e

echo "Compilando proyecto..."
echo ""

# Crear directorio de objetos
mkdir -p obj

# Detectar sistema para flags
if [[ "$OSTYPE" == "darwin"* ]]; then
    LDFLAGS="-pthread"
else
    LDFLAGS="-lrt -pthread"
fi

# Compilar directamente
g++ -std=c++17 -O3 -I./include \
    src/HardwareDetector.cpp \
    src/MatrixMultiplier.cpp \
    src/ProcessManager.cpp \
    src/PerformanceAnalyzer.cpp \
    src/SharedMemory.cpp \
    src/Utils.cpp \
    src/main.cpp \
    -o matrix_mult \
    $LDFLAGS

echo "✓ Compilación exitosa!"
echo ""
echo "Ejecutar con: ./matrix_mult"
