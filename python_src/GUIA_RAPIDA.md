# Guía Rápida - Implementación Python

## ¿Qué se creó?

Una **implementación completa en Python** de multiplicación paralela de matrices para cumplir con los requisitos de la **Actividad en Contacto con el Docente #2**.

---

## Archivos Creados

```
python_src/
├── main.py                      ⭐ PROGRAMA PRINCIPAL (ejecutar este)
├── hardware_detector.py         🔧 Detección de hardware (PLUS)
├── matrix_multiplier.py         🔢 Multiplicación paralela con procesos
├── performance_analyzer.py      📊 Análisis de Ley de Amdahl
├── utils.py                     🛠️ Utilidades varias
├── quick_test.py                ✅ Script de prueba
├── requirements.txt             📦 Dependencias
├── README_PYTHON.md             📖 Documentación completa
└── __init__.py                  📚 Módulo Python
```

---

## Inicio Rápido (3 pasos)

### 1. Instalar dependencias

```bash
cd python_src
pip install -r requirements.txt
```

### 2. Probar que funciona

```bash
python quick_test.py
```

Debe mostrar:
```
✓ TODOS LOS TESTS PASARON
```

### 3. Ejecutar el programa

```bash
python main.py
```

Seleccionar **opción 5** (Benchmark completo) para generar los resultados de la tarea.

---

## Para la Tarea (ACD2)

### Requisitos cumplidos ✅

| Requisito | Implementado | Ubicación |
|-----------|--------------|-----------|
| ✅ Aplicación Python con procesos | Sí | `matrix_multiplier.py` (línea 46) |
| ✅ División de tareas con criterio | Sí | Row-wise partitioning (línea 88) |
| ✅ Variar número de procesos | Sí | `main.py` opción 5 |
| ✅ Medir tiempos y granularidad | Sí | `performance_analyzer.py` |
| ✅ Análisis empírico (Amdahl) | Sí | `performance_analyzer.py` (línea 77) |

### Ejecución para Entrega

**Paso a paso:**

1. **Ejecutar:** `python main.py`
2. **Opción 5:** Benchmark completo
3. **Ingresar tamaño:** 500 (recomendado)
4. **Ingresar procesos:** Enter (usa default: 2,4,6,8)
5. **Esperar resultados** (2-5 minutos)
6. **Capturar pantalla de:**
   - Información del hardware
   - Tabla de resultados
   - Métricas de escalabilidad
7. **Archivo CSV generado:** `benchmark_results.csv`

---

## Diferencias vs Código C++

| Aspecto | C++ (rama main) | Python (esta rama) |
|---------|-----------------|---------------------|
| **Lenguaje** | C++17 | Python 3.8+ |
| **Paralelización** | `fork()` manual | `multiprocessing.Process` |
| **Compilación** | Requiere g++/cmake | No requiere (interpretado) |
| **Portabilidad Windows** | Limitada | Completa |
| **Complejidad código** | ~1000 líneas | ~500 líneas |
| **Velocidad ejecución** | Muy rápida | Rápida (NumPy optimizado) |

---

## Criterio de División Explicado

**División por Filas (Row-wise Partitioning)**

```
Matriz C = A × B (NxN)

Proceso 0 → Filas [0, N/P)
Proceso 1 → Filas [N/P, 2N/P)
Proceso 2 → Filas [2N/P, 3N/P)
...
Proceso P-1 → Filas [(P-1)N/P, N)
```

**Ventajas:**
- Mínima comunicación entre procesos
- Cada proceso trabaja independientemente
- Balance de carga automático

---

## Plus: Detección de Hardware

La implementación incluye detección automática de:

- ✅ Núcleos lógicos y físicos
- ✅ Frecuencia de CPU
- ✅ Tamaños de caché (L1, L2, L3)
- ✅ Memoria RAM total y disponible
- ✅ Sistema operativo y plataforma

**Ver hardware:**
```bash
python -c "from hardware_detector import HardwareDetector; HardwareDetector().print_info()"
```

---

## Solución de Problemas

### Error: "Module not found"

```bash
pip install numpy pandas psutil matplotlib
```

### Error: "Python version"

Requiere Python 3.8+:

```bash
python --version
```

### Programa muy lento

Reducir tamaño de matriz:
- En lugar de 1000 → usar 500
- En lugar de 500 → usar 200

---

## Estructura de Ramas Git

```
main                    → Código C++ original (funcional)
  ├─ python-port        → Rama vacía (primera creada)
  └─ python-implementation → ⭐ Implementación Python completa (ESTA)
```

**Para cambiar entre ramas:**

```bash
# Ver código C++
git checkout main

# Ver código Python
git checkout python-implementation
```

---

## Ejemplo de Resultados Esperados

```
================================================================================
TABLA DE RESULTADOS - Matriz 500x500
================================================================================
# Procesos   Tiempo(s)    Speedup Exp     Speedup Amdahl     Descripción
--------------------------------------------------------------------------------
2            12.3456      1.92x           1.95x              Gruesa (Coarse)
4            7.2341       3.28x           3.42x              Media (Medium)
6            5.4567       4.35x           4.51x              Media (Medium)
8            4.7891       4.96x           5.12x              Fina (Fine)
================================================================================

Fracción Paralelizable (f empírica): 0.9583 (95.83%)
Speedup Máximo Teórico (P→∞): 23.99x
```

---

## Para el Informe PDF

### Contenido Mínimo Requerido

1. **Portada** con datos del estudiante
2. **Captura de hardware** (opción 1 del menú)
3. **Tabla de resultados** (salida de opción 5)
4. **Explicación del criterio de división:** "División por filas"
5. **Análisis de granularidad:** Interpretar Gruesa/Media/Fina
6. **Link al código fuente:** GitHub o Google Drive
7. **Conclusiones** sobre la Ley de Amdahl

---

## Contacto

Esta implementación fue creada específicamente para la **Actividad en Contacto con el Docente #2** de la asignatura **Algoritmos, Análisis y Programación Paralela**.

**Universidad Nacional de Loja**
**Carrera de Computación**

---

## Siguiente Paso

```bash
python main.py
```

¡Selecciona opción 5 y genera tus resultados!
