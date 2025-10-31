#!/bin/bash

# Script para compilar y ejecutar automáticamente
# Uso: ./run.sh

set -e  # Detener si hay errores

# Colores
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # Sin color

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}  Multiplicación de Matrices - C++${NC}"
echo -e "${BLUE}  Script de Compilación Automática${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""

# Detectar sistema operativo
OS="unknown"
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    OS="linux"
    echo -e "${GREEN}✓ Sistema detectado: Linux${NC}"
elif [[ "$OSTYPE" == "darwin"* ]]; then
    OS="macos"
    echo -e "${GREEN}✓ Sistema detectado: macOS${NC}"
elif [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "win32" ]]; then
    OS="windows"
    echo -e "${GREEN}✓ Sistema detectado: Windows${NC}"
else
    echo -e "${YELLOW}⚠ Sistema desconocido, intentando compilar de todas formas...${NC}"
    OS="linux"
fi

# Verificar que g++ esté instalado
if ! command -v g++ &> /dev/null; then
    echo -e "${RED}✗ Error: g++ no está instalado${NC}"
    echo ""
    echo "Por favor, instala g++ primero:"
    echo ""
    if [ "$OS" == "linux" ]; then
        echo "  sudo apt-get update"
        echo "  sudo apt-get install build-essential"
    elif [ "$OS" == "macos" ]; then
        echo "  xcode-select --install"
    fi
    echo ""
    exit 1
fi

echo -e "${GREEN}✓ Compilador g++ encontrado${NC}"
echo ""

# Crear directorio para objetos si no existe
mkdir -p obj

# Flags de compilación
CXX="g++"
CXXFLAGS="-std=c++17 -Wall -Wextra -O3 -I./include"
LDFLAGS=""

# Agregar flags específicos del sistema
if [ "$OS" == "linux" ]; then
    LDFLAGS="-lrt -pthread"
elif [ "$OS" == "macos" ]; then
    LDFLAGS="-pthread"
fi

echo -e "${YELLOW}Compilando archivos fuente...${NC}"
echo ""

# Lista de archivos a compilar
SOURCES=(
    "src/HardwareDetector.cpp"
    "src/MatrixMultiplier.cpp"
    "src/ProcessManager.cpp"
    "src/PerformanceAnalyzer.cpp"
    "src/SharedMemory.cpp"
    "src/Utils.cpp"
    "src/main.cpp"
)

OBJECTS=()

# Compilar cada archivo
for src in "${SOURCES[@]}"; do
    # Extraer nombre del archivo sin extensión
    filename=$(basename "$src" .cpp)
    obj="obj/${filename}.o"

    echo -e "  ${BLUE}→${NC} Compilando ${filename}.cpp..."

    # Compilar
    if $CXX $CXXFLAGS -c "$src" -o "$obj"; then
        OBJECTS+=("$obj")
    else
        echo -e "${RED}✗ Error compilando $src${NC}"
        exit 1
    fi
done

echo ""
echo -e "${YELLOW}Enlazando ejecutable...${NC}"

# Enlazar todos los objetos
if $CXX "${OBJECTS[@]}" -o matrix_mult $LDFLAGS; then
    echo -e "${GREEN}✓ Compilación exitosa!${NC}"
else
    echo -e "${RED}✗ Error al enlazar${NC}"
    exit 1
fi

echo ""
echo -e "${BLUE}========================================${NC}"
echo -e "${GREEN}✓ COMPILACIÓN COMPLETADA${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""

# Preguntar si ejecutar
echo -e "${YELLOW}¿Deseas ejecutar el programa ahora? (s/n)${NC}"
read -r respuesta

if [[ "$respuesta" =~ ^[Ss]$ ]]; then
    echo ""
    echo -e "${BLUE}========================================${NC}"
    echo -e "${BLUE}  EJECUTANDO PROGRAMA${NC}"
    echo -e "${BLUE}========================================${NC}"
    echo ""
    ./matrix_mult
else
    echo ""
    echo -e "${GREEN}Puedes ejecutarlo manualmente con:${NC}"
    echo -e "  ${BLUE}./matrix_mult${NC}"
    echo ""
fi
