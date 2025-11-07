"""
performance_analyzer.py
Análisis de rendimiento usando la Ley de Amdahl.
Calcula speedup, eficiencia, granularidad y métricas de rendimiento.
"""

import pandas as pd
import numpy as np
import matplotlib
matplotlib.use('TkAgg')
import matplotlib.pyplot as plt


class PerformanceAnalyzer:
    """
    Analizador de rendimiento para multiplicación paralela de matrices.
    Implementa la Ley de Amdahl para calcular speedup teórico y experimental.
    """

    def __init__(self):
        self.results = []
        self.sequential_time = None
        self.empirical_f = None  # Fracción paralelizable empírica

    def set_sequential_time(self, time):
        """
        Establece el tiempo de referencia secuencial.

        Args:
            time (float): Tiempo de ejecución secuencial en segundos
        """
        self.sequential_time = time

    def add_result(self, num_processes, parallel_time, matrix_size=None):
        """
        Agrega un resultado de benchmark.

        Args:
            num_processes (int): Número de procesos usados
            parallel_time (float): Tiempo de ejecución paralela en segundos
            matrix_size (int): Tamaño de la matriz (opcional)
        """
        if self.sequential_time is None:
            raise ValueError("Debe establecer sequential_time primero")

        # Calcular métricas
        speedup_exp = self.sequential_time / parallel_time
        efficiency = speedup_exp / num_processes

        # Calcular speedup según Amdahl (si tenemos f empírica)
        speedup_amdahl = None
        if self.empirical_f is not None:
            speedup_amdahl = self.calculate_amdahl_speedup(self.empirical_f, num_processes)

        # Clasificar granularidad
        granularity = self._classify_granularity(efficiency)

        # Calcular GFLOPS (operaciones en punto flotante por segundo)
        gflops = None
        if matrix_size is not None:
            # Multiplicación de matrices: 2*N^3 operaciones (N^3 mult + N^3 sum)
            operations = 2 * (matrix_size ** 3)
            gflops = (operations / parallel_time) / 1e9

        result = {
            'procesos': num_processes,
            'tiempo_s': parallel_time,
            'speedup_exp': speedup_exp,
            'speedup_amdahl': speedup_amdahl,
            'eficiencia': efficiency,
            'granularidad': granularity,
            'gflops': gflops,
            'matrix_size': matrix_size
        }

        self.results.append(result)

        # Si es P=2, calcular f empírica
        if num_processes == 2 and self.empirical_f is None:
            self.empirical_f = self.calculate_empirical_f(speedup_exp)

    def calculate_amdahl_speedup(self, f, P):
        """
        Calcula el speedup teórico según la Ley de Amdahl.

        Ley de Amdahl: S(P) = 1 / ((1-f) + f/P)
        donde:
            f = fracción paralelizable del código
            P = número de procesadores
            (1-f) = fracción secuencial

        Args:
            f (float): Fracción paralelizable (0 a 1)
            P (int): Número de procesadores

        Returns:
            float: Speedup teórico
        """
        if f < 0 or f > 1:
            raise ValueError("f debe estar entre 0 y 1")

        return 1.0 / ((1 - f) + (f / P))

    def calculate_empirical_f(self, speedup_2):
        """
        Calcula la fracción paralelizable empírica usando el speedup con P=2.

        De la ley de Amdahl con P=2:
            S(2) = 1 / ((1-f) + f/2)
            S(2) = 2 / (2 - f)

        Despejando f:
            f = 2 * (1 - 1/S(2))

        Args:
            speedup_2 (float): Speedup experimental con 2 procesadores

        Returns:
            float: Fracción paralelizable empírica
        """
        if speedup_2 <= 0:
            return 0.0

        f = 2 * (1 - 1/speedup_2)
        return max(0.0, min(1.0, f))  # Limitar entre 0 y 1

    def calculate_max_speedup(self, f):
        """
        Calcula el speedup máximo teórico (P → ∞).

        S_max = 1 / (1 - f)

        Args:
            f (float): Fracción paralelizable

        Returns:
            float: Speedup máximo teórico
        """
        if f >= 1.0:
            return float('inf')
        return 1.0 / (1 - f)

    def _classify_granularity(self, efficiency):
        """
        Clasifica la granularidad según la eficiencia.

        Clasificación:
            - Gruesa (Coarse): eficiencia > 0.8
            - Media (Medium): 0.5 < eficiencia ≤ 0.8
            - Fina (Fine): eficiencia ≤ 0.5

        Args:
            efficiency (float): Eficiencia (speedup / num_processes)

        Returns:
            str: Clasificación de granularidad
        """
        if efficiency > 0.8:
            return "Gruesa (Coarse)"
        elif efficiency > 0.5:
            return "Media (Medium)"
        else:
            return "Fina (Fine)"

    def get_results_dataframe(self):
        """
        Retorna los resultados como un DataFrame de pandas.

        Returns:
            pandas.DataFrame: Resultados en formato tabular
        """
        if not self.results:
            return pd.DataFrame()

        df = pd.DataFrame(self.results)

        # Ordenar columnas
        columns = ['procesos', 'tiempo_s', 'speedup_exp']
        if 'speedup_amdahl' in df.columns:
            columns.append('speedup_amdahl')
        columns.extend(['eficiencia', 'granularidad'])
        if 'gflops' in df.columns and df['gflops'].notna().any():
            columns.append('gflops')
        if 'matrix_size' in df.columns:
            columns.append('matrix_size')

        return df[columns]

    def export_to_csv(self, filename='benchmark_results.csv'):
        """
        Exporta resultados a archivo CSV.

        Args:
            filename (str): Nombre del archivo CSV
        """
        df = self.get_results_dataframe()
        df.to_csv(filename, index=False)
        print(f"Resultados exportados a {filename}")

    def print_summary(self):
        """Imprime resumen de resultados"""
        print("\n" + "="*80)
        print("RESUMEN DE RESULTADOS")
        print("="*80)

        if self.sequential_time:
            print(f"Tiempo Secuencial Base: {self.sequential_time:.4f}s")

        if self.empirical_f is not None:
            print(f"Fracción Paralelizable (f empírica): {self.empirical_f:.4f} ({self.empirical_f*100:.2f}%)")
            max_speedup = self.calculate_max_speedup(self.empirical_f)
            print(f"Speedup Máximo Teórico (P→∞): {max_speedup:.2f}x")

        print("\n" + "-"*80)

        df = self.get_results_dataframe()
        if not df.empty:
            # Formatear para mejor visualización
            pd.set_option('display.float_format', lambda x: f'{x:.4f}')
            print(df.to_string(index=False))
        else:
            print("No hay resultados disponibles")

        print("="*80 + "\n")

    def print_comparison_table(self, matrix_size):
        """
        Imprime tabla de comparación formato requerido por la tarea.

        Args:
            matrix_size (int): Tamaño de la matriz
        """
        print("\n" + "="*80)
        print(f"TABLA DE RESULTADOS - Matriz {matrix_size}x{matrix_size}")
        print("="*80)

        print(f"{'# Procesos':<12} {'Tiempo(s)':<12} {'Speedup Exp':<15} {'Speedup Amdahl':<18} {'Descripción':<20}")
        print("-"*80)

        for result in self.results:
            if result['matrix_size'] == matrix_size or result['matrix_size'] is None:
                procesos = result['procesos']
                tiempo = result['tiempo_s']
                speedup_exp = result['speedup_exp']
                speedup_amdahl = result['speedup_amdahl'] if result['speedup_amdahl'] else 0
                granularidad = result['granularidad']

                print(f"{procesos:<12} {tiempo:<12.4f} {speedup_exp:<15.2f}x {speedup_amdahl:<18.2f}x {granularidad:<20}")

        print("="*80)

    def analyze_scalability(self):
        """
        Analiza la escalabilidad del sistema.

        Returns:
            dict: Métricas de escalabilidad
        """
        if len(self.results) < 2:
            return None

        df = self.get_results_dataframe()

        # Eficiencia promedio
        avg_efficiency = df['eficiencia'].mean()

        # ¿Hay escalabilidad débil o fuerte?
        # Escalabilidad fuerte: speedup aumenta linealmente con P
        # Escalabilidad débil: eficiencia se mantiene constante

        max_efficiency = df['eficiencia'].max()
        min_efficiency = df['eficiencia'].min()
        efficiency_drop = max_efficiency - min_efficiency

        scalability_type = "Buena" if efficiency_drop < 0.3 else "Regular" if efficiency_drop < 0.5 else "Pobre"

        return {
            'avg_efficiency': avg_efficiency,
            'max_efficiency': max_efficiency,
            'min_efficiency': min_efficiency,
            'efficiency_drop': efficiency_drop,
            'scalability': scalability_type,
            'empirical_f': self.empirical_f
        }

    def plot_results(self, matrix_size, max_cores):
        """
        Genera grafica 2x2 con analisis de rendimiento.

        Args:
            matrix_size (int): Tamano de matriz usado
            max_cores (int): Numero maximo de nucleos logicos del hardware
        """
        if not self.results:
            print("Error: No hay resultados para graficar")
            return

        if self.sequential_time is None:
            print("Error: No hay tiempo secuencial de referencia")
            return

        # Configurar estilo
        plt.style.use('seaborn-v0_8-darkgrid')
        plt.rcParams['font.size'] = 10
        plt.rcParams['axes.labelsize'] = 11
        plt.rcParams['axes.titlesize'] = 12
        plt.rcParams['legend.fontsize'] = 9

        # Crear figura con 4 subplots
        fig = plt.figure(figsize=(14, 10))

        # Extraer datos
        df = self.get_results_dataframe()
        processes = df['procesos'].values
        speedup_exp = df['speedup_exp'].values
        speedup_amdahl = df['speedup_amdahl'].values if df['speedup_amdahl'].notna().any() else None
        efficiency = df['eficiencia'].values * 100
        time = df['tiempo_s'].values
        gflops = df['gflops'].values if 'gflops' in df.columns and df['gflops'].notna().any() else None

        # Configurar eje X adaptativo
        x_ticks = list(range(2, max_cores + 1, 2))

        # SUBPLOT 1: Speedup
        ax1 = plt.subplot(2, 2, 1)
        ideal_speedup = processes.copy()

        ax1.plot(processes, speedup_exp, 'o-', linewidth=2, markersize=8,
                label='Speedup Experimental', color='#2ecc71')

        if speedup_amdahl is not None:
            ax1.plot(processes, speedup_amdahl, 's--', linewidth=2, markersize=6,
                    label='Speedup Amdahl (Teorico)', color='#e74c3c')

        ax1.plot(processes, ideal_speedup, ':', linewidth=2,
                label='Speedup Ideal (Lineal)', color='#95a5a6')

        ax1.set_xlabel('Numero de Procesos (P)', fontweight='bold')
        ax1.set_ylabel('Speedup (S)', fontweight='bold')
        ax1.set_title('Speedup vs Numero de Procesos', fontweight='bold')
        ax1.set_xticks(x_ticks)
        ax1.set_xlim(0, max_cores + 1)
        ax1.grid(True, alpha=0.3)
        ax1.legend(loc='best', framealpha=0.9)

        # SUBPLOT 2: Eficiencia
        ax2 = plt.subplot(2, 2, 2)
        colors = plt.cm.RdYlGn(efficiency / 100)
        bars = ax2.bar(processes, efficiency, color=colors, edgecolor='black',
                      linewidth=1.5, alpha=0.8, width=1.5)

        ax2.axhline(y=80, color='orange', linestyle='--', linewidth=2,
                   label='Umbral Granularidad Gruesa (80%)')
        ax2.axhline(y=50, color='red', linestyle='--', linewidth=2,
                   label='Umbral Granularidad Media (50%)')

        for bar, eff in zip(bars, efficiency):
            height = bar.get_height()
            ax2.text(bar.get_x() + bar.get_width()/2., height + 2,
                    f'{eff:.1f}%', ha='center', va='bottom', fontsize=9, fontweight='bold')

        ax2.set_xlabel('Numero de Procesos (P)', fontweight='bold')
        ax2.set_ylabel('Eficiencia (%)', fontweight='bold')
        ax2.set_title('Eficiencia vs Numero de Procesos', fontweight='bold')
        ax2.set_xticks(x_ticks)
        ax2.set_xlim(0, max_cores + 1)
        ax2.set_ylim(0, 110)
        ax2.grid(True, alpha=0.3, axis='y')
        ax2.legend(loc='best', framealpha=0.9, fontsize=8)

        # SUBPLOT 3: Tiempo de Ejecucion
        ax3 = plt.subplot(2, 2, 3)
        ax3.plot(processes, time, 'o-', linewidth=2, markersize=10,
                color='#3498db', markerfacecolor='#e74c3c',
                markeredgewidth=2, markeredgecolor='#3498db')

        ax3.set_xlabel('Numero de Procesos (P)', fontweight='bold')
        ax3.set_ylabel('Tiempo de Ejecucion (segundos)', fontweight='bold')
        ax3.set_title('Tiempo de Ejecucion vs Numero de Procesos', fontweight='bold')
        ax3.set_xticks(x_ticks)
        ax3.set_xlim(0, max_cores + 1)
        ax3.grid(True, alpha=0.3)

        # SUBPLOT 4: GFLOPS
        ax4 = plt.subplot(2, 2, 4)
        if gflops is not None:
            ax4.fill_between(processes, gflops, alpha=0.3, color='#9b59b6')
            ax4.plot(processes, gflops, 'o-', linewidth=2, markersize=8,
                    color='#9b59b6', markerfacecolor='white', markeredgewidth=2)

            ax4.set_xlabel('Numero de Procesos (P)', fontweight='bold')
            ax4.set_ylabel('GFLOPS (Giga Operaciones/s)', fontweight='bold')
            ax4.set_title('Rendimiento (GFLOPS) vs Numero de Procesos', fontweight='bold')
            ax4.set_xticks(x_ticks)
            ax4.set_xlim(0, max_cores + 1)
            ax4.grid(True, alpha=0.3)
        else:
            ax4.text(0.5, 0.5, 'GFLOPS no disponible',
                    ha='center', va='center', transform=ax4.transAxes, fontsize=12)
            ax4.set_xticks(x_ticks)
            ax4.set_xlim(0, max_cores + 1)

        # Titulo principal con informacion del sistema
        f_text = f"f: {self.empirical_f:.4f}" if self.empirical_f else "f: N/A"
        suptitle = f'Analisis de Rendimiento - Multiplicacion de Matrices\n'
        suptitle += f'Matriz: {matrix_size}x{matrix_size}  |  Nucleos: {max_cores}  |  {f_text}'

        plt.suptitle(suptitle, fontsize=14, fontweight='bold', y=0.98)
        plt.tight_layout(rect=[0, 0, 1, 0.96])

        print("\nMostrando graficas...")
        print("Cierra la ventana para continuar.")
        plt.show()


# Testing
if __name__ == "__main__":
    # Ejemplo de uso
    analyzer = PerformanceAnalyzer()

    # Simular tiempos (ejemplo)
    t_seq = 10.0
    analyzer.set_sequential_time(t_seq)

    # Agregar resultados simulados
    analyzer.add_result(2, 5.5, 500)
    analyzer.add_result(4, 3.2, 500)
    analyzer.add_result(6, 2.5, 500)
    analyzer.add_result(8, 2.1, 500)

    # Imprimir resumen
    analyzer.print_summary()
    analyzer.print_comparison_table(500)

    # Análisis de escalabilidad
    scalability = analyzer.analyze_scalability()
    print(f"\nEscalabilidad: {scalability['scalability']}")
    print(f"Eficiencia promedio: {scalability['avg_efficiency']:.2%}")
