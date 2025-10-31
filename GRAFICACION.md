# 📊 Guía de Graficación de Resultados

## Flujo de Trabajo Completo

### 1. Ejecutar el Programa C++

```bash
./matrix_mult
```

### 2. Seleccionar Opción 4: BENCHMARK COMPLETO

En el menú, selecciona la opción **4** para ejecutar el benchmark completo.

El programa:
- Ejecutará pruebas con 1, 2, 4, 8, ... procesos
- Mostrará las tablas de resultados
- **Exportará automáticamente** `benchmark_results.csv`

### 3. Generar las Gráficas

```bash
python3 plot_results.py
```

Esto generará 5 archivos PNG:
- `grafica_speedup.png` - Speedup vs Procesos
- `grafica_eficiencia.png` - Eficiencia vs Procesos
- `grafica_tiempo.png` - Tiempo vs Procesos
- `grafica_gflops.png` - GFLOPS vs Procesos
- `grafica_resumen.png` - Panel con las 4 gráficas

## Requisitos Python

Si no tienes las bibliotecas necesarias:

```bash
# Opción 1: pip
pip3 install pandas matplotlib numpy

# Opción 2: conda (si usas miniconda/anaconda)
conda install pandas matplotlib numpy
```

## Formato del CSV

El archivo `benchmark_results.csv` tiene este formato:

```csv
Procesos,Tiempo(s),Speedup_Exp,Speedup_Amdahl,Eficiencia,GFLOPS,Granularidad
1,2.345600,1.0000,1.0000,1.0000,0.895,N/A
2,1.210300,1.9380,1.9608,0.9690,1.735,Gruesa
4,0.645200,3.6360,3.7700,0.9090,3.253,Media
8,0.312100,7.5160,7.0200,0.9395,6.726,Fina
```

## Personalizar las Gráficas

Puedes editar `plot_results.py` para:
- Cambiar colores
- Ajustar tamaños
- Añadir más anotaciones
- Modificar títulos y etiquetas

## Descripción de Gráficas

### 1. Speedup vs Procesos
- **Línea Verde**: Speedup medido experimentalmente
- **Línea Roja**: Speedup teórico según Amdahl
- **Línea Gris**: Speedup ideal (lineal)

**Interpretación**: La línea verde debe estar cerca de la roja (Amdahl). Si está muy por debajo, hay overhead significativo.

### 2. Eficiencia vs Procesos
- **Barras con gradiente**: Verde = alta eficiencia, Rojo = baja
- **Línea Naranja (80%)**: Umbral de granularidad gruesa
- **Línea Roja (50%)**: Umbral de granularidad media

**Interpretación**: Eficiencia > 80% es excelente, indica poco overhead.

### 3. Tiempo de Ejecución vs Procesos
- Muestra la reducción del tiempo al aumentar procesos
- Idealmente debe bajar consistentemente

**Interpretación**: La curva debe mostrar mejora (descenso) al añadir procesos.

### 4. GFLOPS vs Procesos
- Muestra el throughput computacional absoluto
- Más alto = mejor rendimiento

**Interpretación**: Debe aumentar con más procesos, indicando mayor throughput.

## Troubleshooting

### Error: "No se encontró el archivo benchmark_results.csv"
- Asegúrate de haber ejecutado la opción 4 del programa C++
- Verifica que el archivo se generó: `ls -l benchmark_results.csv`

### Error: "ModuleNotFoundError: No module named 'pandas'"
- Instala las dependencias: `pip3 install pandas matplotlib numpy`

### Las gráficas no se muestran en pantalla
- Normal si estás en SSH o sin entorno gráfico
- Las gráficas se guardan como PNG de todas formas

### Error: "cannot connect to X server"
- Trabajas en un entorno sin interfaz gráfica
- Las gráficas PNG se generan correctamente igual
- Para ver las gráficas: copia los PNG a tu computadora local

## Ejemplo Completo

```bash
# 1. Compilar
make

# 2. Ejecutar programa
./matrix_mult
# Selecciona opción 4

# 3. Generar gráficas
python3 plot_results.py

# 4. Ver las gráficas generadas
ls -lh grafica_*.png

# 5. Abrir gráfica resumen (Linux con eog)
eog grafica_resumen.png
```

## Uso Avanzado

### Exportar a otros formatos

Modifica la última línea de cada función `plot_*()` en `plot_results.py`:

```python
# Para PDF
plt.savefig('grafica_speedup.pdf', dpi=300, bbox_inches='tight')

# Para SVG (vectorial)
plt.savefig('grafica_speedup.svg', bbox_inches='tight')
```

### Comparar múltiples ejecuciones

1. Ejecuta el benchmark con diferentes tamaños de matriz
2. Renombra cada CSV: `mv benchmark_results.csv results_500x500.csv`
3. Modifica `plot_results.py` para leer múltiples archivos

---

**¿Preguntas?** Consulta el README.md principal para más información.
