"""
python_src - Multiplicación Paralela de Matrices

Universidad Nacional de Loja
Carrera de Computación
Actividad en Contacto con el Docente #2

Este paquete implementa multiplicación paralela de matrices usando procesos
y analiza el rendimiento mediante la Ley de Amdahl.

Módulos:
    - hardware_detector: Detección de información del hardware
    - matrix_multiplier: Multiplicación secuencial y paralela de matrices
    - performance_analyzer: Análisis de rendimiento y Ley de Amdahl
    - utils: Utilidades generales
    - main: Programa principal con menú interactivo
"""

__version__ = "1.0.0"
__author__ = "Universidad Nacional de Loja - Carrera de Computación"
__date__ = "Noviembre 2025"

# Imports principales
from .hardware_detector import HardwareDetector
from .matrix_multiplier import MatrixMultiplier
from .performance_analyzer import PerformanceAnalyzer

__all__ = [
    'HardwareDetector',
    'MatrixMultiplier',
    'PerformanceAnalyzer'
]
