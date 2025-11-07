"""
matrix_multiplier.py
Multiplicación de matrices usando procesos (paralelización basada en procesos, no hilos).
"""

import os
# Deshabilitar threading interno de NumPy para evitar conflictos con multiprocessing
# DEBE estar ANTES de importar numpy
os.environ['OMP_NUM_THREADS'] = '1'
os.environ['MKL_NUM_THREADS'] = '1'
os.environ['OPENBLAS_NUM_THREADS'] = '1'
os.environ['NUMEXPR_NUM_THREADS'] = '1'

import numpy as np
from multiprocessing import Process, Array
import ctypes
import time


class MatrixMultiplier:
    """
    Multiplicador de matrices con soporte para ejecución secuencial y paralela.
    Usa multiprocessing.Process para paralelización (NO hilos).
    """

    def __init__(self, size, initialize=True):
        """
        Inicializa el multiplicador de matrices.

        Args:
            size (int): Tamaño de las matrices cuadradas (size x size)
            initialize (bool): Si True, inicializa matrices con valores aleatorios
        """
        self.size = size
        self.A = None
        self.B = None
        self.C = None

        if initialize:
            self.initialize_matrices()

    def initialize_matrices(self, seed=None):
        """
        Inicializa matrices A y B con valores aleatorios, C con ceros.

        Args:
            seed (int): Semilla para reproducibilidad (opcional)
        """
        if seed is not None:
            np.random.seed(seed)

        self.A = np.random.rand(self.size, self.size)
        self.B = np.random.rand(self.size, self.size)
        self.C = np.zeros((self.size, self.size))

    def multiply_sequential(self):
        """
        Multiplicación secuencial usando el MISMO algoritmo que la versión paralela.
        Esto permite una comparación justa del speedup real.

        Returns:
            numpy.ndarray: Matriz resultado C = A * B
            float: Tiempo de ejecución en segundos
        """
        start_time = time.time()

        # Usar el mismo algoritmo que la versión paralela para comparación justa
        result = np.zeros((self.size, self.size))

        # Usar operaciones vectorizadas de NumPy para cada fila
        for i in range(self.size):
            # Para cada fila i de A, multiplicar con todas las columnas de B
            # A[i, :] es la fila i (shape: (size,))
            # B es la matriz completa (shape: (size, size))
            # El producto punto de la fila con cada columna da la fila i del resultado
            result[i, :] = np.dot(self.A[i, :], self.B)

        end_time = time.time()

        return result, end_time - start_time

    def multiply_parallel(self, num_processes):
        """
        Multiplicación paralela usando procesos.

        CRITERIO DE DIVISIÓN: División por filas (row-wise partitioning)
        Cada proceso calcula un subconjunto de filas de la matriz resultado.

        Args:
            num_processes (int): Número de procesos a usar

        Returns:
            numpy.ndarray: Matriz resultado C = A * B
            float: Tiempo de ejecución en segundos
        """
        start_time = time.time()

        # Crear arrays compartidos para A, B y C (evitar copiar datos a cada proceso)
        shared_A = Array(ctypes.c_double, self.size * self.size)
        shared_B = Array(ctypes.c_double, self.size * self.size)
        shared_C = Array(ctypes.c_double, self.size * self.size)

        # Copiar datos a memoria compartida
        np_A = np.frombuffer(shared_A.get_obj()).reshape(self.size, self.size)
        np_B = np.frombuffer(shared_B.get_obj()).reshape(self.size, self.size)
        np_C = np.frombuffer(shared_C.get_obj()).reshape(self.size, self.size)

        np_A[:] = self.A
        np_B[:] = self.B
        np_C[:] = 0

        # Calcular distribución de filas entre procesos
        rows_per_process = self.size // num_processes
        processes = []

        for p in range(num_processes):
            # Calcular rango de filas para este proceso
            start_row = p * rows_per_process
            if p == num_processes - 1:
                # Último proceso toma las filas restantes
                end_row = self.size
            else:
                end_row = (p + 1) * rows_per_process

            # Crear y lanzar proceso - ahora pasamos A y B compartidos
            proc = Process(
                target=self._worker_process,
                args=(start_row, end_row, shared_A, shared_B, shared_C, self.size)
            )
            proc.start()
            processes.append(proc)

        # Esperar a que todos los procesos terminen
        for proc in processes:
            proc.join()

        end_time = time.time()

        # Convertir resultado compartido a numpy array
        result = np.frombuffer(shared_C.get_obj()).reshape(self.size, self.size).copy()

        return result, end_time - start_time

    def _worker_process(self, start_row, end_row, shared_A, shared_B, shared_C, size):
        """
        Función ejecutada por cada proceso worker.
        Calcula las filas asignadas de la matriz resultado.

        Args:
            start_row (int): Índice de fila inicial (inclusive)
            end_row (int): Índice de fila final (exclusivo)
            shared_A (Array): Array compartido de lectura para matriz A
            shared_B (Array): Array compartido de lectura para matriz B
            shared_C (Array): Array compartido para escribir resultados
            size (int): Tamaño de las matrices
        """
        # Convertir shared arrays a numpy para facilitar operaciones
        np_A = np.frombuffer(shared_A.get_obj()).reshape(size, size)
        np_B = np.frombuffer(shared_B.get_obj()).reshape(size, size)
        np_C = np.frombuffer(shared_C.get_obj()).reshape(size, size)

        # OPTIMIZACIÓN: Calcular todo el bloque de filas de una vez usando np.dot
        # Esto es más eficiente que calcular fila por fila
        # np_A[start_row:end_row, :] es un bloque de filas (shape: (num_rows, size))
        # np.dot lo multiplica por B completa de una sola vez
        np_C[start_row:end_row, :] = np.dot(np_A[start_row:end_row, :], np_B)

    def multiply_parallel_blocks(self, num_processes, block_size=64):
        """
        Multiplicación paralela con optimización de bloques (tiling).
        Similar a la implementación C++ con cache-friendly block multiplication.

        Args:
            num_processes (int): Número de procesos
            block_size (int): Tamaño del bloque para cache locality

        Returns:
            numpy.ndarray: Matriz resultado
            float: Tiempo de ejecución en segundos
        """
        start_time = time.time()

        # Crear array compartido
        shared_C = Array(ctypes.c_double, self.size * self.size)
        np_shared = np.frombuffer(shared_C.get_obj()).reshape(self.size, self.size)
        np_shared[:] = 0

        # Calcular número total de bloques
        num_blocks = (self.size + block_size - 1) // block_size
        total_blocks = num_blocks * num_blocks

        # Distribuir bloques entre procesos
        blocks_per_process = total_blocks // num_processes
        processes = []

        for p in range(num_processes):
            start_block = p * blocks_per_process
            if p == num_processes - 1:
                end_block = total_blocks
            else:
                end_block = (p + 1) * blocks_per_process

            proc = Process(
                target=self._worker_blocks,
                args=(start_block, end_block, block_size, num_blocks, shared_C)
            )
            proc.start()
            processes.append(proc)

        # Esperar procesos
        for proc in processes:
            proc.join()

        end_time = time.time()

        result = np.frombuffer(shared_C.get_obj()).reshape(self.size, self.size).copy()
        return result, end_time - start_time

    def _worker_blocks(self, start_block, end_block, block_size, num_blocks, shared_C):
        """Worker que procesa bloques para cache locality"""
        np_C = np.frombuffer(shared_C.get_obj()).reshape(self.size, self.size)

        for block_idx in range(start_block, end_block):
            # Convertir índice lineal a coordenadas de bloque (bi, bj)
            bi = block_idx // num_blocks
            bj = block_idx % num_blocks

            # Calcular coordenadas de inicio del bloque
            i_start = bi * block_size
            j_start = bj * block_size

            # Calcular coordenadas de fin (cuidado con bordes)
            i_end = min(i_start + block_size, self.size)
            j_end = min(j_start + block_size, self.size)

            # Multiplicar bloque
            for k_block in range(0, self.size, block_size):
                k_end = min(k_block + block_size, self.size)

                for i in range(i_start, i_end):
                    for j in range(j_start, j_end):
                        value = np_C[i, j]
                        for k in range(k_block, k_end):
                            value += self.A[i, k] * self.B[k, j]
                        np_C[i, j] = value

    def verify_result(self, result_parallel, result_sequential=None):
        """
        Verifica la correctitud del resultado paralelo comparando con secuencial.

        Args:
            result_parallel (numpy.ndarray): Resultado de multiplicación paralela
            result_sequential (numpy.ndarray): Resultado de multiplicación secuencial

        Returns:
            bool: True si los resultados coinciden (dentro de tolerancia numérica)
        """
        if result_sequential is None:
            result_sequential, _ = self.multiply_sequential()

        return np.allclose(result_parallel, result_sequential, rtol=1e-5, atol=1e-8)


# Función auxiliar para testing
def test_multiplier():
    """Prueba básica del multiplicador"""
    print("Probando MatrixMultiplier...")

    size = 100
    mult = MatrixMultiplier(size)

    print(f"Matriz {size}x{size}")

    # Secuencial
    result_seq, time_seq = mult.multiply_sequential()
    print(f"Tiempo secuencial: {time_seq:.4f}s")

    # Paralelo con 2 procesos
    result_par, time_par = mult.multiply_parallel(2)
    print(f"Tiempo paralelo (2 proc): {time_par:.4f}s")

    # Verificar
    if mult.verify_result(result_par, result_seq):
        print("✓ Resultados correctos")
        print(f"Speedup: {time_seq/time_par:.2f}x")
    else:
        print("✗ Error: resultados no coinciden")


if __name__ == "__main__":
    test_multiplier()
