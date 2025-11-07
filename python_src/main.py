#!/usr/bin/env python3
"""
main.py
Programa principal de multiplicación paralela de matrices.
Actividad en Contacto con el Docente #2 - Algoritmos y Programación Paralela

Universidad Nacional de Loja
Carrera de Computación
"""

import sys
import os

# Agregar directorio actual al path para imports
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from hardware_detector import HardwareDetector
from matrix_multiplier import MatrixMultiplier
from performance_analyzer import PerformanceAnalyzer
from utils import (
    clear_screen, print_header, print_section, get_valid_input,
    get_matrix_size, get_process_counts, format_time, pause,
    print_success, print_error, check_requirements
)


class MatrixMultiplicationApp:
    """Aplicación principal de multiplicación de matrices"""

    def __init__(self):
        self.hardware = HardwareDetector()
        self.multiplier = None
        self.analyzer = None
        self.matrix_size = None

    def show_menu(self):
        """Muestra el menú principal"""
        clear_screen()
        print_header("MULTIPLICACIÓN PARALELA DE MATRICES")
        print("Universidad Nacional de Loja - Carrera de Computación")
        print("Actividad en Contacto con el Docente #2")
        print("\nAnálisis de Aceleración mediante Ley de Amdahl")

        print("\n" + "="*80)
        print("MENÚ PRINCIPAL")
        print("="*80)
        print("1. Ver información del hardware")
        print("2. Configurar tamaño de matriz")
        print("3. Ejecutar multiplicación secuencial")
        print("4. Ejecutar multiplicación paralela")
        print("5. Ejecutar benchmark completo (Requerido para tarea)")
        print("6. Ver resultados guardados")
        print("7. Generar graficas")
        print("8. Exportar resultados a CSV")
        print("9. Salir")
        print("="*80)

    def run(self):
        """Ejecuta el programa principal"""
        # Verificar dependencias
        if not check_requirements():
            print("\nNo se puede continuar sin las dependencias necesarias.")
            sys.exit(1)

        while True:
            self.show_menu()
            option = get_valid_input("\nSeleccione una opción: ", valid_options=[1,2,3,4,5,6,7,8,9], input_type=int)

            if option == 1:
                self.show_hardware_info()
            elif option == 2:
                self.configure_matrix_size()
            elif option == 3:
                self.run_sequential()
            elif option == 4:
                self.run_parallel()
            elif option == 5:
                self.run_full_benchmark()
            elif option == 6:
                self.show_results()
            elif option == 7:
                self.generate_plots()
            elif option == 8:
                self.export_results()
            elif option == 9:
                print("\n¡Hasta luego!")
                sys.exit(0)

            pause()

    def show_hardware_info(self):
        """Muestra información del hardware"""
        clear_screen()
        self.hardware.print_info()

    def configure_matrix_size(self):
        """Configura el tamaño de la matriz"""
        clear_screen()
        print_header("CONFIGURACIÓN DE MATRIZ")

        self.matrix_size = get_matrix_size()

        print_section("Inicializando matrices...")
        self.multiplier = MatrixMultiplier(self.matrix_size)
        self.analyzer = PerformanceAnalyzer()

        print_success(f"Matrices {self.matrix_size}x{self.matrix_size} inicializadas")
        print(f"Memoria aproximada: {(3 * self.matrix_size**2 * 8) / (1024**2):.2f} MB")

    def run_sequential(self):
        """Ejecuta multiplicación secuencial"""
        if self.multiplier is None:
            print_error("Primero debe configurar el tamaño de la matriz (opción 2)")
            return

        clear_screen()
        print_header("MULTIPLICACIÓN SECUENCIAL")

        print(f"\nMultiplicando matrices {self.matrix_size}x{self.matrix_size}...")
        print("Por favor espere...\n")

        result, exec_time = self.multiplier.multiply_sequential()

        # Calcular GFLOPS
        operations = 2 * (self.matrix_size ** 3)
        gflops = (operations / exec_time) / 1e9

        print_success("Multiplicación completada")
        print(f"\nTiempo de ejecución: {format_time(exec_time)}")
        print(f"Rendimiento: {gflops:.2f} GFLOPS")

        # Guardar como referencia
        self.analyzer.set_sequential_time(exec_time)

    def run_parallel(self):
        """Ejecuta multiplicación paralela con un número específico de procesos"""
        if self.multiplier is None:
            print_error("Primero debe configurar el tamaño de la matriz (opción 2)")
            return

        if self.analyzer.sequential_time is None:
            print_error("Primero debe ejecutar la versión secuencial (opción 3)")
            return

        clear_screen()
        print_header("MULTIPLICACIÓN PARALELA")

        max_processes = self.hardware.cpu_count_logical
        print(f"\nNúcleos lógicos disponibles: {max_processes}")

        num_processes = get_valid_input(
            f"Número de procesos a usar (1-{max_processes}): ",
            input_type=int
        )

        if num_processes < 1 or num_processes > max_processes:
            print_error(f"Debe estar entre 1 y {max_processes}")
            return

        print(f"\nMultiplicando con {num_processes} procesos...")
        print("Por favor espere...\n")

        result, exec_time = self.multiplier.multiply_parallel(num_processes)

        speedup = self.analyzer.sequential_time / exec_time
        efficiency = speedup / num_processes

        print_success("Multiplicación paralela completada")
        print(f"\nTiempo de ejecución: {format_time(exec_time)}")
        print(f"Speedup: {speedup:.2f}x")
        print(f"Eficiencia: {efficiency:.2%}")

        # Agregar resultado
        self.analyzer.add_result(num_processes, exec_time, self.matrix_size)

    def run_full_benchmark(self):
        """
        Ejecuta benchmark completo según requisitos de la tarea.
        Este es el modo principal para generar los resultados requeridos.
        """
        clear_screen()
        print_header("BENCHMARK COMPLETO - LEY DE AMDAHL")
        print("\nEste modo ejecuta el análisis completo requerido para la tarea:")
        print("  - Multiplicación secuencial (baseline)")
        print("  - Multiplicación paralela con diferentes números de procesos")
        print("  - Cálculo de speedup experimental y teórico (Amdahl)")
        print("  - Análisis de granularidad")
        print("  - Exportación de resultados")

        # Configurar matriz si no existe
        if self.multiplier is None:
            print_section("Paso 1: Configuración de matriz")
            self.matrix_size = get_matrix_size()
            self.multiplier = MatrixMultiplier(self.matrix_size)
            self.analyzer = PerformanceAnalyzer()

        # Obtener números de procesos a probar
        print_section("Paso 2: Configuración de procesos")
        process_counts = get_process_counts()
        print(f"Se probarán con: {process_counts} procesos")

        # Ejecutar secuencial
        print_section("Paso 3: Ejecutando versión secuencial (baseline)")
        print(f"Multiplicando matrices {self.matrix_size}x{self.matrix_size}...")

        result_seq, time_seq = self.multiplier.multiply_sequential()
        self.analyzer.set_sequential_time(time_seq)

        print(f"✓ Tiempo secuencial: {format_time(time_seq)}")

        # Ejecutar versiones paralelas
        print_section("Paso 4: Ejecutando versiones paralelas")

        for i, num_proc in enumerate(process_counts, 1):
            print(f"\n[{i}/{len(process_counts)}] Ejecutando con {num_proc} procesos...")

            result_par, time_par = self.multiplier.multiply_parallel(num_proc)

            # Verificar correctitud (solo para el primer caso)
            if i == 1:
                if self.multiplier.verify_result(result_par, result_seq):
                    print("  ✓ Verificación: Resultado correcto")
                else:
                    print_error("  ✗ Verificación falló: Resultados no coinciden")

            self.analyzer.add_result(num_proc, time_par, self.matrix_size)

            speedup = time_seq / time_par
            print(f"  Tiempo: {format_time(time_par)} | Speedup: {speedup:.2f}x")

        # Mostrar resultados
        print_section("Paso 5: Resultados y Análisis")
        self.analyzer.print_summary()
        self.analyzer.print_comparison_table(self.matrix_size)

        # Análisis de escalabilidad
        scalability = self.analyzer.analyze_scalability()
        if scalability:
            print_section("Análisis de Escalabilidad")
            print(f"Tipo de escalabilidad: {scalability['scalability']}")
            print(f"Eficiencia promedio: {scalability['avg_efficiency']:.2%}")
            print(f"Caída de eficiencia: {scalability['efficiency_drop']:.2%}")
            print(f"Fracción paralelizable (f): {scalability['empirical_f']:.4f}")

        print_success("Benchmark completo finalizado")

        print("\n" + "="*80)
        print("IMPORTANTE PARA LA TAREA:")
        print("="*80)
        print("1. Tome capturas de pantalla de las tablas mostradas")
        print("2. Use las metricas de speedup y granularidad para su informe")
        print("3. Seleccione opcion 7 para generar graficas")
        print("4. Seleccione opcion 8 si necesita exportar a CSV")
        print("="*80)

    def show_results(self):
        """Muestra resultados guardados"""
        if self.analyzer is None or not self.analyzer.results:
            print_error("No hay resultados disponibles")
            return

        clear_screen()
        print_header("RESULTADOS GUARDADOS")

        self.analyzer.print_summary()

        if self.matrix_size:
            self.analyzer.print_comparison_table(self.matrix_size)

    def generate_plots(self):
        """Genera y muestra graficas de rendimiento"""
        if self.analyzer is None or not self.analyzer.results:
            print_error("No hay resultados para graficar")
            return

        if self.analyzer.sequential_time is None:
            print_error("Primero debe ejecutar la version secuencial (opcion 3 o 5)")
            return

        if self.matrix_size is None:
            print_error("No se ha configurado el tamano de matriz")
            return

        clear_screen()
        print_header("GENERAR GRAFICAS")

        print(f"\nGenerando graficas para matriz {self.matrix_size}x{self.matrix_size}")
        print(f"Numero de resultados: {len(self.analyzer.results)}")

        max_cores = self.hardware.cpu_count_logical
        self.analyzer.plot_results(self.matrix_size, max_cores)

    def export_results(self):
        """Exporta resultados a CSV"""
        if self.analyzer is None or not self.analyzer.results:
            print_error("No hay resultados para exportar")
            return

        clear_screen()
        print_header("EXPORTAR RESULTADOS")

        filename = input("\nNombre del archivo (default: benchmark_results.csv): ").strip()
        if not filename:
            filename = 'benchmark_results.csv'

        if not filename.endswith('.csv'):
            filename += '.csv'

        self.analyzer.export_to_csv(filename)
        print_success(f"Resultados exportados a '{filename}'")


def main():
    """Función principal"""
    try:
        app = MatrixMultiplicationApp()
        app.run()
    except KeyboardInterrupt:
        print("\n\nPrograma interrumpido por el usuario.")
        sys.exit(0)
    except Exception as e:
        print_error(f"Error inesperado: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)


if __name__ == "__main__":
    main()
