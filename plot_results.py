#!/usr/bin/env python3
"""
Script para graficar resultados del benchmark de multiplicación de matrices paralela.
Lee el archivo CSV generado por el programa C++ y genera gráficas con matplotlib.

Autor: Proyecto de Programación Paralela
Fecha: 2025-10-31
"""

import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import sys
import os

# Configuración de estilo para gráficas profesionales
plt.style.use('seaborn-v0_8-darkgrid')
plt.rcParams['figure.figsize'] = (12, 8)
plt.rcParams['font.size'] = 10
plt.rcParams['axes.labelsize'] = 12
plt.rcParams['axes.titlesize'] = 14
plt.rcParams['legend.fontsize'] = 10
plt.rcParams['xtick.labelsize'] = 10
plt.rcParams['ytick.labelsize'] = 10

def load_data(filename='benchmark_results.csv'):
    """Carga los datos del archivo CSV."""
    if not os.path.exists(filename):
        print(f"Error: No se encontró el archivo '{filename}'")
        print(f"Ejecuta primero el programa C++ y selecciona la opción 4 (BENCHMARK COMPLETO)")
        sys.exit(1)

    try:
        df = pd.read_csv(filename)
        print(f"✓ Datos cargados exitosamente desde '{filename}'")
        print(f"  Total de mediciones: {len(df)}")
        return df
    except Exception as e:
        print(f"Error al leer el archivo CSV: {e}")
        sys.exit(1)

def plot_speedup(df):
    """Gráfica 1: Speedup vs Número de Procesos"""
    fig, ax = plt.subplots()

    processes = df['Procesos']
    speedup_exp = df['Speedup_Exp']
    speedup_amdahl = df['Speedup_Amdahl']

    # Línea de speedup ideal (diagonal)
    ideal_speedup = processes

    # Graficar las tres líneas
    ax.plot(processes, speedup_exp, 'o-', linewidth=2, markersize=8,
            label='Speedup Experimental', color='#2ecc71')
    ax.plot(processes, speedup_amdahl, 's--', linewidth=2, markersize=6,
            label='Speedup Amdahl (Teórico)', color='#e74c3c')
    ax.plot(processes, ideal_speedup, ':', linewidth=2,
            label='Speedup Ideal (Lineal)', color='#95a5a6')

    # Configuración de ejes
    ax.set_xlabel('Número de Procesos (P)', fontweight='bold')
    ax.set_ylabel('Speedup (S)', fontweight='bold')
    ax.set_title('Speedup vs Número de Procesos\n(Experimental vs Teórico vs Ideal)',
                 fontweight='bold', pad=20)

    # Grid y leyenda
    ax.grid(True, alpha=0.3)
    ax.legend(loc='best', framealpha=0.9)

    # Anotaciones
    max_speedup = speedup_exp.max()
    max_processes = processes[speedup_exp.idxmax()]
    ax.annotate(f'Máximo: {max_speedup:.2f}x\n({max_processes} procesos)',
                xy=(max_processes, max_speedup),
                xytext=(max_processes + 0.5, max_speedup - 0.5),
                arrowprops=dict(arrowstyle='->', color='black', lw=1.5),
                fontsize=10, bbox=dict(boxstyle='round,pad=0.5', facecolor='yellow', alpha=0.7))

    plt.tight_layout()
    plt.savefig('grafica_speedup.png', dpi=300, bbox_inches='tight')
    print("✓ Gráfica guardada: grafica_speedup.png")

    return fig

def plot_efficiency(df):
    """Gráfica 2: Eficiencia vs Número de Procesos"""
    fig, ax = plt.subplots()

    processes = df['Procesos']
    efficiency = df['Eficiencia'] * 100  # Convertir a porcentaje

    # Crear gráfica de barras con degradado de color
    colors = plt.cm.RdYlGn(efficiency / 100)  # Verde para alta eficiencia, rojo para baja
    bars = ax.bar(processes, efficiency, color=colors, edgecolor='black', linewidth=1.5, alpha=0.8)

    # Línea de tendencia para la eficiencia
    ax.plot(processes, efficiency, 'o-', color='#3498db', linewidth=2, markersize=8, label='Tendencia Eficiencia')

    # Zonas de granularidad
    ax.axhspan(80, 110, facecolor='green', alpha=0.15, label='Granularidad Gruesa (> 80%)')
    ax.axhspan(50, 80, facecolor='yellow', alpha=0.2, label='Granularidad Media (50-80%)')
    ax.axhspan(0, 50, facecolor='red', alpha=0.2, label='Granularidad Fina (< 50%)')

    # Configuración de ejes
    ax.set_xlabel('Número de Procesos (P)', fontweight='bold')
    ax.set_ylabel('Eficiencia (%)', fontweight='bold')
    ax.set_title('Eficiencia y Granularidad vs Número de Procesos',
                 fontweight='bold', pad=20)
    ax.set_ylim(0, 110)

    # Etiquetas sobre las barras
    for bar, eff in zip(bars, efficiency):
        height = bar.get_height()
        ax.text(bar.get_x() + bar.get_width()/2., height + 2,
                f'{eff:.1f}%', ha='center', va='bottom', fontsize=9, fontweight='bold')

    # Grid y leyenda
    ax.grid(True, alpha=0.3, axis='y')
    ax.legend(loc='best', framealpha=0.9)

    plt.tight_layout()
    plt.savefig('grafica_eficiencia.png', dpi=300, bbox_inches='tight')
    print("✓ Gráfica guardada: grafica_eficiencia.png")

    return fig

def plot_execution_time(df):
    """Gráfica 3: Tiempo de Ejecución vs Número de Procesos"""
    fig, ax = plt.subplots()

    processes = df['Procesos']
    time = df['Tiempo(s)']

    # Gráfica de línea con marcadores
    ax.plot(processes, time, 'o-', linewidth=2, markersize=10,
            color='#3498db', markerfacecolor='#e74c3c', markeredgewidth=2, markeredgecolor='#3498db')

    # Configuración de ejes
    ax.set_xlabel('Número de Procesos (P)', fontweight='bold')
    ax.set_ylabel('Tiempo de Ejecución (segundos)', fontweight='bold')
    ax.set_title('Tiempo de Ejecución vs Número de Procesos\n(Reducción de Tiempo por Paralelización)',
                 fontweight='bold', pad=20)

    # Grid
    ax.grid(True, alpha=0.3)

    # Anotaciones para primer y último punto
    ax.annotate(f'Secuencial: {time.iloc[0]:.3f}s',
                xy=(processes.iloc[0], time.iloc[0]),
                xytext=(processes.iloc[0] + 0.5, time.iloc[0] + 0.1),
                arrowprops=dict(arrowstyle='->', color='black', lw=1.5),
                fontsize=10, bbox=dict(boxstyle='round,pad=0.5', facecolor='lightcoral', alpha=0.7))

    ax.annotate(f'Paralelo ({processes.iloc[-1]} proc): {time.iloc[-1]:.3f}s',
                xy=(processes.iloc[-1], time.iloc[-1]),
                xytext=(processes.iloc[-1] - 1, time.iloc[-1] + 0.1),
                arrowprops=dict(arrowstyle='->', color='black', lw=1.5),
                fontsize=10, bbox=dict(boxstyle='round,pad=0.5', facecolor='lightgreen', alpha=0.7))

    plt.tight_layout()
    plt.savefig('grafica_tiempo.png', dpi=300, bbox_inches='tight')
    print("✓ Gráfica guardada: grafica_tiempo.png")

    return fig

def plot_gflops(df):
    """Gráfica 4: GFLOPS vs Número de Procesos"""
    fig, ax = plt.subplots()

    processes = df['Procesos']
    gflops = df['GFLOPS']

    # Gráfica de área con línea
    ax.fill_between(processes, gflops, alpha=0.3, color='#9b59b6')
    ax.plot(processes, gflops, 'o-', linewidth=2, markersize=8,
            color='#9b59b6', markerfacecolor='white', markeredgewidth=2)

    # Configuración de ejes
    ax.set_xlabel('Número de Procesos (P)', fontweight='bold')
    ax.set_ylabel('GFLOPS (Giga Operaciones/s)', fontweight='bold')
    ax.set_title('Rendimiento (GFLOPS) vs Número de Procesos\n(Throughput Computacional)',
                 fontweight='bold', pad=20)

    # Grid
    ax.grid(True, alpha=0.3)

    # Marcar el máximo rendimiento
    max_gflops = gflops.max()
    max_proc = processes[gflops.idxmax()]
    ax.annotate(f'Máximo: {max_gflops:.2f} GFLOPS\n({max_proc} procesos)',
                xy=(max_proc, max_gflops),
                xytext=(max_proc - 1, max_gflops - 0.5),
                arrowprops=dict(arrowstyle='->', color='black', lw=1.5),
                fontsize=10, bbox=dict(boxstyle='round,pad=0.5', facecolor='cyan', alpha=0.7))

    plt.tight_layout()
    plt.savefig('grafica_gflops.png', dpi=300, bbox_inches='tight')
    print("✓ Gráfica guardada: grafica_gflops.png")

    return fig

def create_summary_figure(df):
    """Crea una figura resumen con las 4 gráficas en un solo archivo"""
    fig = plt.figure(figsize=(16, 12))

    processes = df['Procesos']

    # Subplot 1: Speedup
    ax1 = plt.subplot(2, 2, 1)
    ax1.plot(processes, df['Speedup_Exp'], 'o-', linewidth=2, markersize=8, label='Experimental', color='#2ecc71')
    ax1.plot(processes, df['Speedup_Amdahl'], 's--', linewidth=2, markersize=6, label='Amdahl', color='#e74c3c')
    ax1.plot(processes, processes, ':', linewidth=2, label='Ideal', color='#95a5a6')
    ax1.set_xlabel('Procesos', fontweight='bold')
    ax1.set_ylabel('Speedup', fontweight='bold')
    ax1.set_title('Speedup vs Procesos', fontweight='bold')
    ax1.legend()
    ax1.grid(True, alpha=0.3)

    # Subplot 2: Eficiencia
    ax2 = plt.subplot(2, 2, 2)
    efficiency = df['Eficiencia'] * 100
    colors = plt.cm.RdYlGn(efficiency / 100)
    ax2.bar(processes, efficiency, color=colors, edgecolor='black', linewidth=1.5, alpha=0.8)
    ax2.plot(processes, efficiency, 'o-', color='#3498db', linewidth=2, markersize=5, label='Tendencia')
    ax2.axhspan(80, 110, facecolor='green', alpha=0.15, label='G. Gruesa (>80%)')
    ax2.axhspan(50, 80, facecolor='yellow', alpha=0.2, label='G. Media (50-80%)')
    ax2.axhspan(0, 50, facecolor='red', alpha=0.2, label='G. Fina (<50%)')
    ax2.set_xlabel('Procesos', fontweight='bold')
    ax2.set_ylabel('Eficiencia (%)', fontweight='bold')
    ax2.set_title('Eficiencia y Granularidad', fontweight='bold')
    ax2.legend()
    ax2.grid(True, alpha=0.3, axis='y')

    # Subplot 3: Tiempo
    ax3 = plt.subplot(2, 2, 3)
    ax3.plot(processes, df['Tiempo(s)'], 'o-', linewidth=2, markersize=10, color='#3498db')
    ax3.set_xlabel('Procesos', fontweight='bold')
    ax3.set_ylabel('Tiempo (s)', fontweight='bold')
    ax3.set_title('Tiempo de Ejecución vs Procesos', fontweight='bold')
    ax3.grid(True, alpha=0.3)

    # Subplot 4: GFLOPS
    ax4 = plt.subplot(2, 2, 4)
    ax4.fill_between(processes, df['GFLOPS'], alpha=0.3, color='#9b59b6')
    ax4.plot(processes, df['GFLOPS'], 'o-', linewidth=2, markersize=8, color='#9b59b6')
    ax4.set_xlabel('Procesos', fontweight='bold')
    ax4.set_ylabel('GFLOPS', fontweight='bold')
    ax4.set_title('Rendimiento (GFLOPS) vs Procesos', fontweight='bold')
    ax4.grid(True, alpha=0.3)

    plt.suptitle('Análisis Completo de Paralelización - Multiplicación de Matrices',
                 fontsize=16, fontweight='bold', y=0.995)
    plt.tight_layout()
    plt.savefig('grafica_resumen.png', dpi=300, bbox_inches='tight')
    print("✓ Gráfica resumen guardada: grafica_resumen.png")

    return fig

def main():
    """Función principal"""
    print("=" * 60)
    print("  GRAFICADOR DE RESULTADOS - MULTIPLICACIÓN DE MATRICES")
    print("=" * 60)
    print()

    # Cargar datos
    df = load_data()
    print()

    # Generar todas las gráficas
    print("Generando gráficas...")
    print("-" * 60)

    plot_speedup(df)
    plot_efficiency(df)
    plot_execution_time(df)
    plot_gflops(df)
    create_summary_figure(df)

    print("-" * 60)
    print()
    print("✓ ¡Todas las gráficas generadas exitosamente!")
    print()
    print("Archivos creados:")
    print("  1. grafica_speedup.png      - Speedup vs Procesos")
    print("  2. grafica_eficiencia.png   - Eficiencia vs Procesos")
    print("  3. grafica_tiempo.png       - Tiempo vs Procesos")
    print("  4. grafica_gflops.png       - GFLOPS vs Procesos")
    print("  5. grafica_resumen.png      - Resumen con las 4 gráficas")
    print()
    print("=" * 60)

    # Mostrar gráficas en pantalla (opcional)
    try:
        print("\nMostrando gráficas en pantalla...")
        print("(Cierra las ventanas para terminar)")
        plt.show()
    except Exception as e:
        print(f"\nNota: No se pudieron mostrar las gráficas en pantalla: {e}")
        print("Las gráficas fueron guardadas como archivos PNG.")

if __name__ == "__main__":
    main()
