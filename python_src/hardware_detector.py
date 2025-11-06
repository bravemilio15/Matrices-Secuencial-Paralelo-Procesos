"""
hardware_detector.py
Detecta información del hardware (CPU, caché, memoria) del sistema.
"""

import psutil
import platform
import os
import re


class HardwareDetector:
    """Detecta y reporta información del hardware del sistema"""

    def __init__(self):
        self.cpu_count_logical = os.cpu_count()
        self.cpu_count_physical = psutil.cpu_count(logical=False)
        self.cpu_freq = psutil.cpu_freq()
        self.memory = psutil.virtual_memory()
        self.cache_info = self._detect_cache_sizes()

    def _detect_cache_sizes(self):
        """Detecta tamaños de caché L1, L2, L3"""
        cache = {'L1': None, 'L2': None, 'L3': None}

        system = platform.system()

        if system == 'Linux':
            cache = self._detect_cache_linux()
        elif system == 'Windows':
            cache = self._detect_cache_windows()
        elif system == 'Darwin':  # macOS
            cache = self._detect_cache_macos()

        return cache

    def _detect_cache_linux(self):
        """Detecta caché en Linux leyendo /proc/cpuinfo o sysfs"""
        cache = {'L1': None, 'L2': None, 'L3': None}

        try:
            # Intentar leer desde sysfs (más confiable)
            cpu_path = '/sys/devices/system/cpu/cpu0/cache/'

            for index in range(4):  # Hasta 4 niveles de caché
                index_path = f'{cpu_path}index{index}/'
                if os.path.exists(index_path):
                    with open(f'{index_path}level', 'r') as f:
                        level = f.read().strip()
                    with open(f'{index_path}size', 'r') as f:
                        size_str = f.read().strip()

                    # Convertir tamaño (ej: "32K", "256K", "3M")
                    size_kb = self._parse_cache_size(size_str)
                    cache[f'L{level}'] = size_kb

        except Exception:
            # Fallback: leer /proc/cpuinfo
            try:
                with open('/proc/cpuinfo', 'r') as f:
                    content = f.read()

                    # Buscar líneas de caché
                    if 'cache size' in content:
                        match = re.search(r'cache size\s*:\s*(\d+)\s*KB', content)
                        if match:
                            cache['L2'] = int(match.group(1))
            except Exception:
                pass

        return cache

    def _detect_cache_windows(self):
        """Detecta caché en Windows usando wmic"""
        cache = {'L1': None, 'L2': None, 'L3': None}

        try:
            import subprocess
            result = subprocess.run(
                ['wmic', 'cpu', 'get', 'L2CacheSize,L3CacheSize', '/format:list'],
                capture_output=True,
                text=True,
                timeout=5
            )

            output = result.stdout

            # Parsear salida
            l2_match = re.search(r'L2CacheSize=(\d+)', output)
            l3_match = re.search(r'L3CacheSize=(\d+)', output)

            if l2_match:
                cache['L2'] = int(l2_match.group(1))
            if l3_match:
                cache['L3'] = int(l3_match.group(1))

            # Estimar L1 (típicamente 32KB por core)
            cache['L1'] = 32

        except Exception:
            pass

        return cache

    def _detect_cache_macos(self):
        """Detecta caché en macOS usando sysctl"""
        cache = {'L1': None, 'L2': None, 'L3': None}

        try:
            import subprocess

            # L1 data cache
            result = subprocess.run(
                ['sysctl', '-n', 'hw.l1dcachesize'],
                capture_output=True,
                text=True,
                timeout=5
            )
            if result.returncode == 0:
                cache['L1'] = int(result.stdout.strip()) // 1024  # bytes to KB

            # L2 cache
            result = subprocess.run(
                ['sysctl', '-n', 'hw.l2cachesize'],
                capture_output=True,
                text=True,
                timeout=5
            )
            if result.returncode == 0:
                cache['L2'] = int(result.stdout.strip()) // 1024

            # L3 cache
            result = subprocess.run(
                ['sysctl', '-n', 'hw.l3cachesize'],
                capture_output=True,
                text=True,
                timeout=5
            )
            if result.returncode == 0:
                cache['L3'] = int(result.stdout.strip()) // 1024

        except Exception:
            pass

        return cache

    def _parse_cache_size(self, size_str):
        """Convierte string de tamaño (32K, 3M) a KB"""
        match = re.match(r'(\d+)([KMG]?)', size_str)
        if match:
            value = int(match.group(1))
            unit = match.group(2)

            if unit == 'K' or unit == '':
                return value
            elif unit == 'M':
                return value * 1024
            elif unit == 'G':
                return value * 1024 * 1024

        return None

    def get_optimal_block_size(self):
        """Calcula tamaño óptimo de bloque basado en caché L1"""
        l1_size = self.cache_info.get('L1')

        if l1_size:
            # block_size = sqrt(L1_cache / (3 * sizeof(double)))
            # sizeof(double) = 8 bytes
            import math
            l1_bytes = l1_size * 1024
            block_size = int(math.sqrt(l1_bytes / (3 * 8)))

            # Limitar entre 32 y 256
            return max(32, min(256, block_size))
        else:
            # Default si no se puede detectar
            return 64

    def print_info(self):
        """Imprime información del hardware"""
        print("=" * 70)
        print("INFORMACIÓN DEL HARDWARE")
        print("=" * 70)

        # Sistema operativo
        print(f"Sistema Operativo:     {platform.system()} {platform.release()}")
        print(f"Plataforma:            {platform.machine()}")
        print(f"Procesador:            {platform.processor()}")

        # CPU
        print(f"\nNúcleos Lógicos:       {self.cpu_count_logical}")
        print(f"Núcleos Físicos:       {self.cpu_count_physical}")

        if self.cpu_freq:
            print(f"Frecuencia CPU:        {self.cpu_freq.current:.2f} MHz")
            if self.cpu_freq.max > 0:
                print(f"Frecuencia Máxima:     {self.cpu_freq.max:.2f} MHz")

        # Caché
        print(f"\nCaché L1:              {self.cache_info['L1']} KB" if self.cache_info['L1'] else "\nCaché L1:              No detectada")
        print(f"Caché L2:              {self.cache_info['L2']} KB" if self.cache_info['L2'] else "Caché L2:              No detectada")
        print(f"Caché L3:              {self.cache_info['L3']} KB" if self.cache_info['L3'] else "Caché L3:              No detectada")

        # Memoria
        total_mem_gb = self.memory.total / (1024**3)
        available_mem_gb = self.memory.available / (1024**3)
        print(f"\nMemoria Total:         {total_mem_gb:.2f} GB")
        print(f"Memoria Disponible:    {available_mem_gb:.2f} GB")
        print(f"Uso de Memoria:        {self.memory.percent}%")

        # Block size recomendado
        block_size = self.get_optimal_block_size()
        print(f"\nTamaño de Bloque Óptimo: {block_size}")

        print("=" * 70)

    def get_info_dict(self):
        """Retorna información del hardware como diccionario"""
        return {
            'os': f"{platform.system()} {platform.release()}",
            'platform': platform.machine(),
            'processor': platform.processor(),
            'logical_cores': self.cpu_count_logical,
            'physical_cores': self.cpu_count_physical,
            'cpu_freq_current': self.cpu_freq.current if self.cpu_freq else None,
            'cpu_freq_max': self.cpu_freq.max if self.cpu_freq else None,
            'cache_l1_kb': self.cache_info['L1'],
            'cache_l2_kb': self.cache_info['L2'],
            'cache_l3_kb': self.cache_info['L3'],
            'memory_total_gb': self.memory.total / (1024**3),
            'memory_available_gb': self.memory.available / (1024**3),
            'memory_percent': self.memory.percent,
            'optimal_block_size': self.get_optimal_block_size()
        }


if __name__ == "__main__":
    # Test del módulo
    detector = HardwareDetector()
    detector.print_info()
