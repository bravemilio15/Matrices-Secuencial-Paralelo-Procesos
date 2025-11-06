#!/usr/bin/env python3
"""
quick_test.py
Script de prueba rápida para verificar que todo funciona correctamente.
"""

import sys
import os

# Agregar directorio actual al path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

print("="*80)
print("PRUEBA RÁPIDA - Multiplicación Paralela de Matrices")
print("="*80)

# Test 1: Verificar dependencias
print("\n[1/5] Verificando dependencias...")
try:
    import numpy as np
    import pandas as pd
    import psutil
    import matplotlib
    print("✓ Todas las dependencias instaladas")
except ImportError as e:
    print(f"✗ Falta dependencia: {e}")
    print("Instalar con: pip install -r requirements.txt")
    sys.exit(1)

# Test 2: Hardware Detector
print("\n[2/5] Probando HardwareDetector...")
try:
    from hardware_detector import HardwareDetector
    hw = HardwareDetector()
    print(f"✓ CPU: {hw.cpu_count_logical} núcleos lógicos, {hw.cpu_count_physical} físicos")
    print(f"  Caché L1: {hw.cache_info['L1']} KB" if hw.cache_info['L1'] else "  Caché L1: No detectada")
    print(f"  Bloque óptimo: {hw.get_optimal_block_size()}")
except Exception as e:
    print(f"✗ Error: {e}")
    sys.exit(1)

# Test 3: Matrix Multiplier
print("\n[3/5] Probando MatrixMultiplier...")
try:
    from matrix_multiplier import MatrixMultiplier

    # Test con matriz pequeña
    mult = MatrixMultiplier(50, initialize=True)

    # Secuencial
    result_seq, time_seq = mult.multiply_sequential()
    print(f"✓ Multiplicación secuencial: {time_seq:.4f}s")

    # Paralelo
    result_par, time_par = mult.multiply_parallel(2)
    print(f"✓ Multiplicación paralela (2 proc): {time_par:.4f}s")

    # Verificar
    if mult.verify_result(result_par, result_seq):
        print(f"✓ Verificación correcta")
        speedup = time_seq / time_par
        print(f"  Speedup: {speedup:.2f}x")
    else:
        print("✗ Error: resultados no coinciden")
        sys.exit(1)

except Exception as e:
    print(f"✗ Error: {e}")
    import traceback
    traceback.print_exc()
    sys.exit(1)

# Test 4: Performance Analyzer
print("\n[4/5] Probando PerformanceAnalyzer...")
try:
    from performance_analyzer import PerformanceAnalyzer

    analyzer = PerformanceAnalyzer()
    analyzer.set_sequential_time(time_seq)
    analyzer.add_result(2, time_par, 50)

    print(f"✓ Fracción paralelizable: {analyzer.empirical_f:.4f}")

    speedup_amdahl = analyzer.calculate_amdahl_speedup(analyzer.empirical_f, 2)
    print(f"✓ Speedup Amdahl (P=2): {speedup_amdahl:.2f}x")

except Exception as e:
    print(f"✗ Error: {e}")
    sys.exit(1)

# Test 5: Utils
print("\n[5/5] Probando Utils...")
try:
    from utils import format_time, print_success

    formatted = format_time(1.234)
    print(f"✓ Formato de tiempo: {formatted}")

except Exception as e:
    print(f"✗ Error: {e}")
    sys.exit(1)

# Resumen
print("\n" + "="*80)
print("✓ TODOS LOS TESTS PASARON")
print("="*80)
print("\nEl programa está listo para usar.")
print("Ejecute: python main.py")
print("="*80)
