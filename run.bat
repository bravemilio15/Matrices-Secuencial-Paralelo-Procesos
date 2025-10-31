@echo off
REM Script de compilacion para Windows
REM Uso: run.bat

echo ========================================
echo   Multiplicacion de Matrices - C++
echo   Script de Compilacion para Windows
echo ========================================
echo.

REM Verificar que g++ este instalado
where g++ >nul 2>nul
if %errorlevel% neq 0 (
    echo [ERROR] g++ no esta instalado
    echo.
    echo Para instalar g++ en Windows:
    echo 1. Instala MinGW desde: https://sourceforge.net/projects/mingw/
    echo    O usa MSYS2: https://www.msys2.org/
    echo 2. O instala Visual Studio con herramientas de C++
    echo.
    pause
    exit /b 1
)

echo [OK] Compilador g++ encontrado
echo.

REM Crear directorio de objetos
if not exist obj mkdir obj

echo Compilando archivos fuente...
echo.

REM Compilar cada archivo
echo   - Compilando HardwareDetector.cpp...
g++ -std=c++17 -O3 -I./include -c src/HardwareDetector.cpp -o obj/HardwareDetector.o
if %errorlevel% neq 0 goto error

echo   - Compilando MatrixMultiplier.cpp...
g++ -std=c++17 -O3 -I./include -c src/MatrixMultiplier.cpp -o obj/MatrixMultiplier.o
if %errorlevel% neq 0 goto error

echo   - Compilando ProcessManager.cpp...
g++ -std=c++17 -O3 -I./include -c src/ProcessManager.cpp -o obj/ProcessManager.o
if %errorlevel% neq 0 goto error

echo   - Compilando PerformanceAnalyzer.cpp...
g++ -std=c++17 -O3 -I./include -c src/PerformanceAnalyzer.cpp -o obj/PerformanceAnalyzer.o
if %errorlevel% neq 0 goto error

echo   - Compilando SharedMemory.cpp...
g++ -std=c++17 -O3 -I./include -c src/SharedMemory.cpp -o obj/SharedMemory.o
if %errorlevel% neq 0 goto error

echo   - Compilando Utils.cpp...
g++ -std=c++17 -O3 -I./include -c src/Utils.cpp -o obj/Utils.o
if %errorlevel% neq 0 goto error

echo   - Compilando main.cpp...
g++ -std=c++17 -O3 -I./include -c src/main.cpp -o obj/main.o
if %errorlevel% neq 0 goto error

echo.
echo Enlazando ejecutable...

REM Enlazar (sin -lrt que no existe en Windows)
g++ obj/HardwareDetector.o obj/MatrixMultiplier.o obj/ProcessManager.o obj/PerformanceAnalyzer.o obj/SharedMemory.o obj/Utils.o obj/main.o -o matrix_mult.exe
if %errorlevel% neq 0 goto error

echo.
echo ========================================
echo [OK] COMPILACION EXITOSA
echo ========================================
echo.
echo ADVERTENCIA: En Windows, la paralelizacion con procesos
echo tiene limitaciones. Para mejor rendimiento, usa WSL.
echo.
echo Ejecutar con: matrix_mult.exe
echo.
pause
exit /b 0

:error
echo.
echo [ERROR] Compilacion fallida
echo.
pause
exit /b 1
