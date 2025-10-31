# Multiplicación de Matrices Paralela con Procesos

Programa en C++ que implementa multiplicación de matrices optimizada usando **procesos** y **optimización por bloques** (blocking/tiling). Incluye análisis de rendimiento con la **Ley de Amdahl**.

## ⚡ Quick Start (FÁCIL - Sin instalar nada)

### 🐧 Linux / 🍎 macOS

**Opción 1: Compilar y ejecutar automáticamente (MÁS SIMPLE)**

```bash
./run.sh
```

Ese único comando:
- ✅ Detecta tu sistema operativo
- ✅ Verifica que tengas g++ instalado
- ✅ Compila todo automáticamente
- ✅ Pregunta si quieres ejecutar

**Opción 2: Solo compilar (sin ejecutar)**

```bash
./compile.sh
./matrix_mult
```

**¿No tienes g++ instalado?** Sigue las instrucciones que aparecerán en pantalla.

---

### 🪟 Windows

**⚠️ IMPORTANTE:** El programa usa `fork()` que NO existe nativamente en Windows. Tienes 3 opciones:

**1️⃣ WSL (Recomendada - Funciona al 100%)**
```bash
wsl --install        # En PowerShell como Administrador
# Luego, dentro de WSL:
./run.sh
```

**2️⃣ Git Bash (Simple pero limitado)**
```bash
./run.sh             # Abre Git Bash en la carpeta
```

**3️⃣ CMD nativo (Con limitaciones serias)**
```cmd
run.bat              # Compila pero SIN paralelización real
matrix_mult.exe
```

📖 **Instrucciones detalladas:** Lee `INSTRUCCIONES_WINDOWS.txt`

### Otras opciones (si ya conoces make/cmake):

<details>
<summary>Haz clic aquí para ver opciones avanzadas</summary>

**Con Makefile (Linux/macOS):**
```bash
make
./matrix_mult
```

**Con CMake (multiplataforma):**
```bash
mkdir build && cd build
cmake ..
cmake --build .
./matrix_mult
```
</details>

## 🚀 Características

- ✅ **Multiplicación optimizada por bloques**: Aprovecha la localidad de caché (5-20x más rápido)
- ✅ **Paralelización con procesos**: Usa `fork()` en Linux/macOS (mejor que hilos para este caso)
- ✅ **Detección automática de hardware**: Núcleos, caché, SO
- ✅ **Análisis de Amdahl**: Calcula fracción paralelizable (f) empíricamente
- ✅ **Benchmark completo**: Prueba con 1, 2, 4, 8, ..., P_max procesos
- ✅ **Multiplataforma**: Compatible con Linux, macOS, Windows

## 📋 Requisitos Mínimos

Solo necesitas:
- **g++** instalado (compilador de C++)
- **Linux** o **macOS** (Windows tiene limitaciones en paralelización)

**¿No tienes g++?**

```bash
# Ubuntu/Debian/ZorinOS
sudo apt-get update && sudo apt-get install build-essential

# macOS
xcode-select --install
```

**El script `./run.sh` detectará automáticamente si falta g++ y te dirá qué comando ejecutar.**

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
- **Exporta automáticamente** los resultados a `benchmark_results.csv` para graficación

#### 5. Configurar tamaño de matrices

- Permite cambiar el tamaño de las matrices (NxN)
- Rango válido: 10 a 10000

#### 6. Mostrar configuración actual

- Tamaño de matriz
- Tamaño de bloques (auto-optimizado)
- Número máximo de procesos
- Núcleos disponibles

## 📊 Visualización de Resultados

### Generar Gráficas

Después de ejecutar el **BENCHMARK COMPLETO** (opción 4), se genera automáticamente el archivo `benchmark_results.csv`. Para visualizar los resultados:

```bash
python3 plot_results.py
```

Este script genera 5 gráficas PNG:

1. **grafica_speedup.png** - Speedup vs Procesos (Experimental vs Teórico vs Ideal)
2. **grafica_eficiencia.png** - Eficiencia vs Procesos (con umbrales de granularidad)
3. **grafica_tiempo.png** - Tiempo de Ejecución vs Procesos
4. **grafica_gflops.png** - Rendimiento (GFLOPS) vs Procesos
5. **grafica_resumen.png** - Panel con las 4 gráficas principales

### Requisitos para Graficación

```bash
# Instalar dependencias de Python (si no están instaladas)
pip3 install pandas matplotlib numpy

# O usando conda
conda install pandas matplotlib numpy
```

**Nota:** Las gráficas se guardan automáticamente como archivos PNG de alta resolución (300 DPI).

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
├── plot_results.py             # Script Python para graficación
├── benchmark_results.csv       # Resultados exportados (generado)
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
