"""
utils.py
Utilidades generales para el programa de multiplicación de matrices.
"""

import sys
import os


def clear_screen():
    """Limpia la pantalla de la consola"""
    os.system('cls' if os.name == 'nt' else 'clear')


def print_header(title):
    """
    Imprime un encabezado formateado.

    Args:
        title (str): Título a mostrar
    """
    width = 80
    print("\n" + "="*width)
    print(title.center(width))
    print("="*width)


def print_section(title):
    """
    Imprime un título de sección.

    Args:
        title (str): Título de la sección
    """
    print("\n" + "-"*80)
    print(title)
    print("-"*80)


def get_valid_input(prompt, valid_options=None, input_type=int):
    """
    Obtiene entrada validada del usuario.

    Args:
        prompt (str): Mensaje a mostrar
        valid_options (list): Lista de opciones válidas (opcional)
        input_type (type): Tipo de dato esperado (int, float, str)

    Returns:
        Valor ingresado y validado
    """
    while True:
        try:
            user_input = input(prompt)

            # Convertir al tipo especificado
            if input_type == int:
                value = int(user_input)
            elif input_type == float:
                value = float(user_input)
            else:
                value = user_input

            # Validar contra opciones si se especificaron
            if valid_options is not None:
                if value not in valid_options:
                    print(f"Opción inválida. Debe ser una de: {valid_options}")
                    continue

            return value

        except ValueError:
            print(f"Entrada inválida. Debe ser de tipo {input_type.__name__}")
        except KeyboardInterrupt:
            print("\n\nOperación cancelada por el usuario.")
            sys.exit(0)


def get_matrix_size():
    """
    Solicita al usuario el tamaño de la matriz.

    Returns:
        int: Tamaño de la matriz
    """
    print("\nIngrese el tamaño de la matriz (N para matriz NxN):")
    print("  - Pequeño (100-300): Rápido, ideal para pruebas")
    print("  - Mediano (500-1000): Balance entre tiempo y precisión")
    print("  - Grande (>1000): Más preciso pero más lento")

    while True:
        size = get_valid_input("Tamaño: ", input_type=int)
        if size < 10:
            print("El tamaño debe ser al menos 10")
        elif size > 5000:
            confirm = input(f"Tamaño {size} puede tardar mucho. ¿Continuar? (s/n): ")
            if confirm.lower() == 's':
                return size
        else:
            return size


def get_process_counts():
    """
    Solicita al usuario los números de procesos a probar.

    Returns:
        list: Lista de números de procesos
    """
    print("\n¿Cuántos configuraciones de procesos quiere probar?")
    print("  - Ingrese números separados por comas (ej: 2,4,6,8)")
    print("  - O presione Enter para usar valores por defecto [2,4,6,8]")

    user_input = input("Números de procesos: ").strip()

    if not user_input:
        return [2, 4, 6, 8]

    try:
        # Parsear entrada
        process_counts = [int(x.strip()) for x in user_input.split(',')]

        # Validar
        if any(p < 1 for p in process_counts):
            print("Todos los valores deben ser ≥ 1. Usando valores por defecto.")
            return [2, 4, 6, 8]

        # Ordenar y eliminar duplicados
        process_counts = sorted(set(process_counts))
        return process_counts

    except ValueError:
        print("Formato inválido. Usando valores por defecto.")
        return [2, 4, 6, 8]


def format_time(seconds):
    """
    Formatea tiempo en segundos a formato legible.

    Args:
        seconds (float): Tiempo en segundos

    Returns:
        str: Tiempo formateado
    """
    if seconds < 1:
        return f"{seconds*1000:.2f}ms"
    elif seconds < 60:
        return f"{seconds:.2f}s"
    else:
        minutes = int(seconds // 60)
        secs = seconds % 60
        return f"{minutes}m {secs:.2f}s"


def print_progress_bar(iteration, total, prefix='', suffix='', length=50, fill='█'):
    """
    Imprime una barra de progreso en la consola.

    Args:
        iteration (int): Iteración actual
        total (int): Total de iteraciones
        prefix (str): Prefijo del mensaje
        suffix (str): Sufijo del mensaje
        length (int): Longitud de la barra
        fill (str): Carácter de relleno
    """
    percent = 100 * (iteration / float(total))
    filled_length = int(length * iteration // total)
    bar = fill * filled_length + '-' * (length - filled_length)

    print(f'\r{prefix} |{bar}| {percent:.1f}% {suffix}', end='')

    if iteration == total:
        print()


def pause():
    """Pausa la ejecución esperando input del usuario"""
    input("\nPresione Enter para continuar...")


def confirm_action(message):
    """
    Solicita confirmación del usuario.

    Args:
        message (str): Mensaje de confirmación

    Returns:
        bool: True si el usuario confirma, False en caso contrario
    """
    response = input(f"{message} (s/n): ").strip().lower()
    return response in ['s', 'si', 'y', 'yes']


def print_warning(message):
    """
    Imprime un mensaje de advertencia.

    Args:
        message (str): Mensaje a mostrar
    """
    print(f"\n⚠ ADVERTENCIA: {message}")


def print_error(message):
    """
    Imprime un mensaje de error.

    Args:
        message (str): Mensaje a mostrar
    """
    print(f"\n✗ ERROR: {message}")


def print_success(message):
    """
    Imprime un mensaje de éxito.

    Args:
        message (str): Mensaje a mostrar
    """
    print(f"\n✓ ÉXITO: {message}")


def format_number(num, decimals=2):
    """
    Formatea un número con separadores de miles.

    Args:
        num (float): Número a formatear
        decimals (int): Número de decimales

    Returns:
        str: Número formateado
    """
    if decimals == 0:
        return f"{int(num):,}".replace(',', '.')
    else:
        return f"{num:,.{decimals}f}".replace(',', 'X').replace('.', ',').replace('X', '.')


def check_requirements():
    """
    Verifica que las dependencias necesarias estén instaladas.

    Returns:
        bool: True si todas las dependencias están disponibles
    """
    required_modules = {
        'numpy': 'numpy',
        'pandas': 'pandas',
        'psutil': 'psutil',
        'matplotlib': 'matplotlib'
    }

    missing = []

    for module_name, package_name in required_modules.items():
        try:
            __import__(module_name)
        except ImportError:
            missing.append(package_name)

    if missing:
        print_error("Faltan las siguientes dependencias:")
        for package in missing:
            print(f"  - {package}")
        print("\nInstale las dependencias con:")
        print(f"  pip install {' '.join(missing)}")
        return False

    return True


def create_output_directory(directory='output'):
    """
    Crea un directorio para guardar resultados si no existe.

    Args:
        directory (str): Nombre del directorio

    Returns:
        str: Ruta del directorio
    """
    if not os.path.exists(directory):
        os.makedirs(directory)
        print(f"Directorio '{directory}' creado.")

    return directory


if __name__ == "__main__":
    # Test de utilidades
    print_header("Test de Utilidades")

    print_section("Formateo de tiempo")
    print(f"0.05s = {format_time(0.05)}")
    print(f"2.5s = {format_time(2.5)}")
    print(f"125s = {format_time(125)}")

    print_section("Formateo de números")
    print(f"1234567.89 = {format_number(1234567.89)}")

    print_section("Barra de progreso")
    import time
    for i in range(11):
        print_progress_bar(i, 10, prefix='Progreso:', suffix='Completo')
        time.sleep(0.1)

    print_success("Test completado")
