# Multiplicación Paralela de Matrices en Python

**Universidad Nacional de Loja**
**Carrera de Computación**
**Actividad en Contacto con el Docente #2**
**Asignatura:** Algoritmos, Análisis y Programación Paralela

---

## Descripción

Este programa implementa **multiplicación paralela de matrices** usando **procesos** (NO hilos) en Python. Analiza el rendimiento mediante la **Ley de Amdahl** para diferentes configuraciones de procesos.

### Características

- ✅ Multiplicación de matrices usando `multiprocessing.Process`
- ✅ División de tareas por filas (criterio claro de particionamiento)
- ✅ Variación de número de procesos (2, 4, 6, 8, etc.)
- ✅ Medición de tiempos y análisis de granularidad
- ✅ Cálculo de speedup experimental y teórico (Amdahl)
- ✅ Detección de hardware (CPU, caché, memoria)
- ✅ Exportación de resultados a CSV
- ✅ Generación de gráficas con matplotlib

---

## Requisitos

### Software Necesario

- **Python 3.8+** (requerido para `multiprocessing.shared_memory`)
- pip (gestor de paquetes de Python)

### Dependencias

Instalar con:

```bash
cd python_src
pip install -r requirements.txt
```

O manualmente:

```bash
pip install numpy pandas psutil matplotlib tabulate
```

---

## Instalación

### 1. Clonar el repositorio

```bash
git clone <url-del-repositorio>
cd Matrices
```

### 2. Cambiar a la rama Python

```bash
git checkout python-implementation
```

### 3. Instalar dependencias

```bash
cd python_src
pip install -r requirements.txt
```

---

## Uso

### Ejecutar el programa principal

```bash
cd python_src
python main.py
```

O en sistemas Linux/macOS:

```bash
cd python_src
chmod +x main.py
./main.py
```

### Menú Principal

```
================================================================================
           MULTIPLICACIÓN PARALELA DE MATRICES
================================================================================
Universidad Nacional de Loja - Carrera de Computación
Actividad en Contacto con el Docente #2

Análisis de Aceleración mediante Ley de Amdahl

================================================================================
MENÚ PRINCIPAL
================================================================================
1. Ver información del hardware
2. Configurar tamaño de matriz
3. Ejecutar multiplicación secuencial
4. Ejecutar multiplicación paralela
5. Ejecutar benchmark completo (Requerido para tarea)
6. Ver resultados guardados
7. Exportar resultados a CSV
8. Salir
================================================================================
```

---

## Flujo de Trabajo Recomendado

### Para completar la tarea académica:

**Opción 1: Benchmark Automático (Recomendado)**

1. Ejecutar `python main.py`
2. Seleccionar opción **5** (Benchmark completo)
3. Ingresar tamaño de matriz (ej: 500)
4. Ingresar números de procesos (ej: 2,4,6,8 o Enter para default)
5. Esperar a que termine
6. Tomar **capturas de pantalla** de los resultados
7. Los resultados se guardan automáticamente en `benchmark_results.csv`

**Opción 2: Manual (más control)**

1. Opción 1: Ver información del hardware
2. Opción 2: Configurar tamaño de matriz
3. Opción 3: Ejecutar versión secuencial (baseline)
4. Opción 4: Ejecutar paralelo (repetir para 2, 4, 6, 8 procesos)
5. Opción 6: Ver resultados
6. Opción 7: Exportar a CSV

---

## Estructura de Archivos

```
python_src/
├── main.py                      # Programa principal con menú interactivo
├── hardware_detector.py         # Detección de CPU, caché, memoria
├── matrix_multiplier.py         # Multiplicación secuencial y paralela
├── performance_analyzer.py      # Análisis de Ley de Amdahl
├── utils.py                     # Utilidades (menús, formateo, validación)
├── requirements.txt             # Dependencias de Python
├── README_PYTHON.md             # Este archivo
└── benchmark_results.csv        # Resultados exportados (generado)
```

---

## Criterio de División de Tareas

**División por Filas (Row-wise Partitioning)**

Cada proceso calcula un subconjunto de filas de la matriz resultado:

- **Proceso 0:** Calcula filas 0 a N/P
- **Proceso 1:** Calcula filas N/P a 2N/P
- **Proceso k:** Calcula filas k·(N/P) a (k+1)·(N/P)

**Ventajas:**
- Minimiza comunicación entre procesos
- Cada proceso trabaja independientemente
- Distribución de carga equilibrada

---

## Análisis de Granularidad

El programa clasifica la granularidad según la **eficiencia**:

| Eficiencia | Granularidad | Interpretación |
|------------|--------------|----------------|
| > 0.8 | Gruesa (Coarse) | Excelente paralelización |
| 0.5 - 0.8 | Media (Medium) | Paralelización aceptable |
| < 0.5 | Fina (Fine) | Overhead alto, baja eficiencia |

**Fórmula:** `Eficiencia = Speedup / Número_de_Procesos`

---

## Ley de Amdahl

### Fórmula

```
S(P) = 1 / ((1-f) + f/P)
```

Donde:
- **S(P):** Speedup con P procesadores
- **f:** Fracción paralelizable del código (0 ≤ f ≤ 1)
- **P:** Número de procesadores
- **(1-f):** Fracción secuencial

### Cálculo de f Empírica

El programa calcula la fracción paralelizable usando el speedup con P=2:

```
f = 2 * (1 - 1/S(2))
```

### Speedup Máximo

```
S_max = 1 / (1-f)    (cuando P → ∞)
```

---

## Ejemplo de Resultados

```
================================================================================
TABLA DE RESULTADOS - Matriz 500x500
================================================================================
# Procesos   Tiempo(s)    Speedup Exp     Speedup Amdahl     Descripción
--------------------------------------------------------------------------------
2            5.2341       1.91x           1.92x              Gruesa (Coarse)
4            3.1245       3.20x           3.35x              Media (Medium)
6            2.4567       4.07x           4.23x              Media (Medium)
8            2.1234       4.71x           4.71x              Fina (Fine)
================================================================================
```

---

## Generar Gráficas

Después de ejecutar el benchmark, generar gráficas con:

```bash
python ../plot_results.py
```

Esto genera:
- `grafica_speedup.png` - Speedup experimental vs Amdahl vs ideal
- `grafica_eficiencia.png` - Eficiencia vs número de procesos
- `grafica_tiempo.png` - Tiempo de ejecución vs procesos
- `grafica_gflops.png` - Rendimiento (GFLOPS) vs procesos
- `grafica_resumen.png` - Dashboard con todas las gráficas

---

## Testing Individual de Módulos

Cada módulo se puede probar independientemente:

```bash
# Test de hardware detector
python hardware_detector.py

# Test de multiplicador
python matrix_multiplier.py

# Test de analizador
python performance_analyzer.py

# Test de utilidades
python utils.py
```

---

## Comparación Python vs C++

| Aspecto | Python | C++ (versión original) |
|---------|--------|------------------------|
| **Desarrollo** | Rápido (~300 líneas) | Más complejo (~1000+ líneas) |
| **Paralelización** | `multiprocessing` nativo | `fork()` + IPC manual |
| **Portabilidad** | Multiplataforma | Problemas en Windows |
| **Performance** | Buena (NumPy usa C/BLAS) | Excelente |
| **Mantenimiento** | Fácil | Más difícil |

---

## Notas Importantes

### Limitaciones de Windows

En Windows, `multiprocessing` tiene algunas limitaciones:
- Los procesos hijos **no heredan memoria** (usa `spawn` en lugar de `fork`)
- Las matrices A y B se copian a cada proceso (overhead de memoria)
- En Linux/macOS es más eficiente (`fork` comparte memoria)

### Tamaños de Matriz Recomendados

- **100-300:** Rápido, ideal para pruebas
- **500-1000:** Balance entre tiempo y precisión (recomendado para tarea)
- **1000+:** Más lento pero muestra mejor el paralelismo

### Número de Procesos

- **Óptimo:** Igual al número de núcleos físicos
- **Máximo:** Número de núcleos lógicos
- **Más allá:** Overhead por cambio de contexto

---

## Resolución de Problemas

### Error: "Module not found"

```bash
pip install -r requirements.txt
```

### Error: "Python version too old"

Requiere Python 3.8+. Verificar:

```bash
python --version
```

### Resultados incorrectos

Verificar que la versión paralela coincide con secuencial:

```python
# En matrix_multiplier.py
mult = MatrixMultiplier(100)
result_seq, _ = mult.multiply_sequential()
result_par, _ = mult.multiply_parallel(2)
print(mult.verify_result(result_par, result_seq))  # Debe ser True
```

### Programa muy lento

Reducir tamaño de matriz o número de procesos.

---

## Para el Informe PDF (Tarea)

### Información a Incluir

1. **Captura del hardware** (Opción 1 del menú)
2. **Tabla de resultados** (salida de opción 5)
3. **Gráficas generadas** (plot_results.py)
4. **Criterio de división:** "División por filas - cada proceso calcula N/P filas"
5. **Análisis de granularidad:** Interpretar los resultados
6. **Link al código fuente:** Google Drive o similar

### Formato de Entrega

- **Nombre:** `[Apellido]_[Nombre]_ACD2.pdf`
- **Tamaño máximo:** 5MB
- **Contenido:** Capturas + explicación en sus palabras

---

## Contacto y Soporte

Para preguntas sobre el código:
- Revisar comentarios en los archivos fuente
- Ejecutar tests individuales de módulos
- Verificar que las dependencias estén instaladas

---

## Licencia

Este código es para uso académico en la Universidad Nacional de Loja.

---

**Última actualización:** Noviembre 2025
