# Multiplicación de Matrices Paralela con Procesos

Programa en C++ que implementa multiplicación de matrices optimizada usando **procesos** y **optimización por bloques** (blocking/tiling). Incluye análisis de rendimiento con la **Ley de Amdahl**.

## Quick start

Sigue estos pasos mínimos para compilar y ejecutar en Linux (o macOS):

```bash
# Crear directorio de build y compilar con CMake (recomendado)
mkdir -p build && cd build
cmake ..
cmake --build .

# Ejecutar el binario (desde build/ o la raíz si usaste Makefile)
./matrix_mult
```

Si prefieres usar el `Makefile` (Linux/macOS):

```bash
make
./matrix_mult
```

Lee la sección "🛠️ Compilación" más abajo para alternativas y opciones.

## 🚀 Características

- ✅ **Multiplicación optimizada por bloques**: Aprovecha la localidad de caché (5-20x más rápido)
- ✅ **Paralelización con procesos**: Usa `fork()` en Linux/macOS (mejor que hilos para este caso)
- ✅ **Detección automática de hardware**: Núcleos, caché, SO
- ✅ **Análisis de Amdahl**: Calcula fracción paralelizable (f) empíricamente
- ✅ **Benchmark completo**: Prueba con 1, 2, 4, 8, ..., P_max procesos
- ✅ **Multiplataforma**: Compatible con Linux, macOS, Windows

## 📋 Requisitos

- **Compilador**: g++ o clang con soporte C++17
- **Sistema operativo**:
  - Linux (probado en ZorinOS/Ubuntu)
  - macOS
  - Windows (con algunas limitaciones en paralelización)
- **CMake**: 3.10 o superior (opcional, para compilación alternativa)

## 🛠️ Compilación

### Opción 1: Makefile (Linux/macOS)

```bash
# Compilar
make

# Compilar y ejecutar
make run

# Limpiar archivos compilados
make clean

# Ver información del sistema
make info

# Ver ayuda
make help
```

### Opción 2: CMake (Multiplataforma)

```bash
# Crear directorio de build
mkdir build
cd build

# Configurar con CMake
cmake ..

# Compilar
cmake --build .

# Ejecutar
./matrix_mult
```

## 📖 Uso

### Menú Principal

```
========================================
    MULTIPLICACION DE MATRICES EN C++
    Procesos + Optimizacion por Bloques
========================================
1. Mostrar informacion del hardware
2. Ejecutar multiplicacion SECUENCIAL
3. Ejecutar multiplicacion PARALELA (P_max)
4. Ejecutar BENCHMARK COMPLETO (comparacion)
5. Configurar tamanio de matrices
6. Mostrar configuracion actual
7. Salir
========================================
```

### Opciones Explicadas

#### 1. Mostrar información del hardware

Muestra:
- Sistema operativo
- Modelo de CPU
- Núcleos lógicos y físicos
- Tamaños de caché (L1, L2, L3)
- Tamaño de bloque óptimo calculado

#### 2. Multiplicación SECUENCIAL

- Ejecuta una sola vez con 1 proceso (P=1)
- Muestra tabla con tiempo y GFLOPS
- Usa optimización por bloques

#### 3. Multiplicación PARALELA

- Ejecuta con el máximo de procesos disponibles (P_max)
- Compara contra tiempo secuencial
- Muestra speedup experimental

#### 4. BENCHMARK COMPLETO

- Ejecuta múltiples veces: P = 1, 2, 4, 8, ..., P_max
- Genera 2 tablas:
  1. **Tabla Secuencial** (P=1)
  2. **Tabla Paralela** (comparación de todos los P)
- Calcula:
  - Fracción paralelizable (f) empíricamente
  - Speedup experimental vs. Speedup teórico (Amdahl)
  - Eficiencia
  - Granularidad (Gruesa/Media/Fina)
  - Speedup máximo teórico

#### 5. Configurar tamaño de matrices

- Permite cambiar el tamaño de las matrices (NxN)
- Rango válido: 10 a 10000

#### 6. Mostrar configuración actual

- Tamaño de matriz
- Tamaño de bloques (auto-optimizado)
- Número máximo de procesos
- Núcleos disponibles

## 📊 Ejemplo de Salida

### Información del Hardware

```
========================================
    INFORMACION DEL HARDWARE
========================================
Sistema Operativo:         Linux
Modelo de CPU:             Intel Core i7-9700K
Nucleos Logicos:           8
Nucleos Fisicos (est.):    8
Cache L1:                  32.00 KB
Cache L2:                  256.00 KB
Cache L3:                  12.00 MB
Tamanio de bloque optimo:  64x64
========================================
```

### Tabla de Benchmark Completo

```
========================================
    TABLA PARALELA - COMPARACION
========================================
| Procesos   | Tiempo (s)   | Speedup Exp | Speedup Amdahl | Eficiencia   | Granularidad |
| 1          | 2.345600     | 1.00x       | 1.00x          | 100.0%       | N/A          |
| 2          | 1.210300     | 1.94x       | 1.96x          | 97.0%        | Gruesa       |
| 4          | 0.645200     | 3.64x       | 3.77x          | 91.0%        | Media        |
| 8          | 0.312100     | 7.52x       | 7.02x          | 94.0%        | Fina         |
========================================

========================================
    FRACCION PARALELIZABLE (f)
========================================
f (teorico asumido):       0.980 (98.0% paralelizable)
f (empirico medido):       0.973 (97.3% paralelizable)
Metodo usado:              Calculado de S(2) experimental
========================================

========================================
    ANALISIS FINAL
========================================
Mejor speedup observado:        7.52x con 8 procesos
Speedup maximo teorico (Amdahl): 37.04x (P->infinito)
Limitacion por codigo secuencial: 2.7%
========================================
```

## 🧮 Conceptos Técnicos

### Optimización por Bloques (Blocking/Tiling)

En lugar de multiplicar matrices elemento por elemento:

```
C[i][j] = Σ A[i][k] * B[k][j]
```

Se dividen en bloques pequeños que caben en caché L1:

```
Para cada bloque de C:
  Cargar bloque de A en caché
  Cargar bloque de B en caché
  Calcular bloque de C
```

**Ventajas:**
- Reduce cache misses de ~90% a ~10%
- Mejora hasta 20x en rendimiento
- Tamaño de bloque calculado automáticamente según caché L1

### Ley de Amdahl

La fracción paralelizable (f) determina el speedup máximo:

```
S(P) = 1 / ((1-f) + f/P)
S_max = 1 / (1-f)
```

**Ejemplo:**
- Si f = 0.95 (95% paralelizable, 5% secuencial)
- Speedup máximo = 1/0.05 = 20x (incluso con infinitos procesos)

El programa calcula **f empíricamente** usando la medición con P=2:

```
f = 2 * (1 - 1/S(2))
```

### Granularidad

Indica el balance entre cómputo y comunicación:

- **Gruesa** (Eficiencia > 80%): Poco overhead, mucho cómputo
- **Media** (50% < Eficiencia ≤ 80%): Balance
- **Fina** (Eficiencia ≤ 50%): Mucho overhead, poco cómputo

## 📁 Estructura del Proyecto

```
proyecto/
├── include/
│   ├── Config.h                # Constantes y estructuras
│   ├── HardwareDetector.h      # Detección de hardware
│   ├── MatrixMultiplier.h      # Multiplicación optimizada
│   ├── ProcessManager.h        # Gestión de procesos
│   ├── PerformanceAnalyzer.h   # Análisis de rendimiento
│   ├── SharedMemory.h          # Memoria compartida (IPC)
│   └── Utils.h                 # Utilidades
│
├── src/
│   ├── main.cpp                # Programa principal
│   ├── HardwareDetector.cpp
│   ├── MatrixMultiplier.cpp
│   ├── ProcessManager.cpp
│   ├── PerformanceAnalyzer.cpp
│   ├── SharedMemory.cpp
│   └── Utils.cpp
│
├── Makefile                    # Compilación (Linux/macOS)
├── CMakeLists.txt              # Compilación (multiplataforma)
└── README.md                   # Este archivo
```

## 🐛 Solución de Problemas

### Error: "No se pudo crear memoria compartida"

**Causa:** Límites del sistema para memoria compartida.

**Solución (Linux):**
```bash
# Ver límites actuales
ipcs -lm

# Aumentar límite (como root)
sudo sysctl -w kernel.shmmax=1073741824
sudo sysctl -w kernel.shmall=268435456
```

### Error de compilación: "pthread.h not found"

**Solución:**
```bash
# Ubuntu/Debian
sudo apt-get install build-essential

# macOS (instalar Xcode Command Line Tools)
xcode-select --install
```

### El programa se bloquea en paralelo

**Causa:** Procesos hijos no terminan correctamente.

**Solución:** Verificar que no hay procesos zombies:
```bash
ps aux | grep matrix_mult
killall -9 matrix_mult
```

## 📝 Notas

- El tamaño de bloque se calcula automáticamente según el caché L1 de tu CPU
- En Windows, la implementación paralela tiene limitaciones (usa CreateProcess en lugar de fork)
- Para matrices muy grandes (>5000x5000), considera aumentar la memoria compartida del sistema
- Los mejores resultados se obtienen con matrices de tamaño múltiplo del tamaño de bloque

## 👨‍💻 Autor

Proyecto desarrollado para el curso de Programación Paralela.

## 📄 Licencia

Este proyecto es de código abierto para fines educativos.
