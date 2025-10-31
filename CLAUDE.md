# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is a C++ parallel matrix multiplication program that implements **process-based parallelization** with **block optimization (tiling)** for cache efficiency. The program demonstrates Amdahl's Law by calculating the parallelizable fraction empirically through benchmarking.

**Key characteristics:**
- Uses `fork()` for process-based parallelization (Linux/macOS native, limited on Windows)
- Implements block/tiling optimization to maximize cache locality (L1 cache)
- Analyzes performance using Amdahl's Law with empirical fraction calculation
- Provides interactive menu for different execution modes

## Visualization and Plotting

The program includes automatic CSV export and Python plotting capabilities.

### Generating Results

When running the benchmark (option 4 in the menu), results are automatically exported to `benchmark_results.csv` with format:
```
Procesos,Tiempo(s),Speedup_Exp,Speedup_Amdahl,Eficiencia,GFLOPS,Granularidad
```

### Creating Plots

```bash
python3 plot_results.py
```

Generates 5 PNG files:
- **grafica_speedup.png**: Speedup (Experimental vs Amdahl vs Ideal)
- **grafica_eficiencia.png**: Efficiency vs Processes (with granularity thresholds)
- **grafica_tiempo.png**: Execution Time vs Processes
- **grafica_gflops.png**: Performance (GFLOPS) vs Processes
- **grafica_resumen.png**: Summary dashboard with all 4 plots

**Dependencies**: pandas, matplotlib, numpy
```bash
pip3 install pandas matplotlib numpy
```

## Build Commands

### Quick Compilation & Execution (Recommended)
```bash
./run.sh              # Auto-detects OS, compiles, and optionally runs
./compile.sh          # Compile only (no execution)
```

### Using Makefile (Linux/macOS)
```bash
make                  # Compile
make run             # Compile and execute
make clean           # Remove compiled files
make rebuild         # Clean and recompile
make info            # Show system information
```

### Using CMake (Cross-platform)
```bash
mkdir build && cd build
cmake ..
cmake --build .
./matrix_mult
```

### Compiler Requirements
- **Compiler:** g++ with C++17 support
- **Flags:** `-std=c++17 -Wall -Wextra -O3`
- **Linux linking:** `-lrt -pthread`
- **macOS linking:** `-pthread` (no `-lrt`)

## Architecture Overview

### Core Components

**MatrixMultiplier** (`include/MatrixMultiplier.h`, `src/MatrixMultiplier.cpp`)
- Implements block-optimized matrix multiplication
- `multiply_sequential()`: Single-process blocked multiplication
- `multiply_block()`: Computes a single block using cache-friendly nested loops
- Block size is auto-calculated based on L1 cache size for optimal performance

**ProcessManager** (`include/ProcessManager.h`, `src/ProcessManager.cpp`)
- Manages parallel execution using `fork()` (POSIX) or `CreateProcess` (Windows)
- `multiply_parallel()`: Distributes blocks across processes
- Work distribution: divides total blocks evenly among processes
- Uses shared memory for IPC (inter-process communication)

**SharedMemory** (`include/SharedMemory.h`, `src/SharedMemory.cpp`)
- RAII wrapper for shared memory (IPC mechanism)
- Linux/macOS: `shm_open()` + `mmap()`
- Windows: `CreateFileMapping()` + `MapViewOfFile()`
- Three shared memory segments: matrix A, matrix B, matrix C (result)

**PerformanceAnalyzer** (`include/PerformanceAnalyzer.h`, `src/PerformanceAnalyzer.cpp`)
- Calculates Amdahl's Law metrics
- Computes empirical parallelizable fraction: `f = 2 * (1 - 1/S(2))`
- Determines granularity (Coarse/Medium/Fine) based on efficiency
- Generates comparison tables (sequential vs parallel)

**HardwareDetector** (`include/HardwareDetector.h`, `src/HardwareDetector.cpp`)
- Detects CPU cores (logical and physical)
- Reads cache sizes (L1, L2, L3) from `/proc/cpuinfo` (Linux) or system APIs
- Calculates optimal block size: `block_size = sqrt(L1_cache_size / (3 * sizeof(double)))`

### Parallelization Strategy

1. **Block division:** Matrix divided into `(N/block_size) x (N/block_size)` blocks
2. **Process distribution:** Total blocks distributed evenly across P processes
3. **Each process computes:** A contiguous range of blocks
4. **Communication:** Via shared memory (all matrices in shared memory)
5. **Synchronization:** Parent waits for all children with `wait()` / `waitpid()`

### Amdahl's Law Implementation

- **Theoretical f:** Initially assumes 98% parallelizable (Config::THEORETICAL_F = 0.98)
- **Empirical f:** Calculated from P=2 measurement: `f = 2 * (1 - 1/S(2))`
- **Speedup formula:** `S(P) = 1 / ((1-f) + f/P)`
- **Max speedup:** `S_max = 1 / (1-f)` (as P → ∞)

### Configuration Constants

See `include/Config.h`:
- `DEFAULT_MATRIX_SIZE = 500` (user can change via menu)
- `MIN_BLOCK_SIZE = 32`, `MAX_BLOCK_SIZE = 256`, `DEFAULT_BLOCK_SIZE = 64`
- Block size is auto-optimized based on L1 cache

## Development Workflow

### Running Tests
This project doesn't include a formal test suite. To verify correctness:
1. Run sequential and parallel with same matrix size
2. Compare results (both should produce same output)
3. Use small matrix size (e.g., 100x100) for quick validation

### Debugging
```bash
# Compile with debug symbols
g++ -std=c++17 -g -O0 -I./include src/*.cpp -o matrix_mult -pthread -lrt

# Check for memory leaks with valgrind
valgrind --leak-check=full ./matrix_mult

# Debug shared memory issues
ipcs -m                    # List shared memory segments
ipcrm -m <shmid>          # Remove orphaned segment
```

### Common Issues

**"No se pudo crear memoria compartida"**
- Increase shared memory limits (Linux):
  ```bash
  sudo sysctl -w kernel.shmmax=1073741824
  sudo sysctl -w kernel.shmall=268435456
  ```

**Processes become zombies**
- Check with: `ps aux | grep matrix_mult`
- Kill with: `killall -9 matrix_mult`

**Windows limitations**
- `fork()` doesn't exist on Windows natively
- Use WSL (Windows Subsystem for Linux) for full functionality
- Native Windows build uses `CreateProcess` (limited parallelization)

## Code Patterns to Follow

### Memory Management
- Use RAII pattern (constructor allocates, destructor frees)
- SharedMemory class already follows this
- For matrices: `MatrixMultiplier` manages allocation/deallocation

### Adding New Features
When modifying parallelization logic:
1. Update `ProcessManager::worker_process()` for child process work
2. Ensure shared memory is properly sized in `setup_shared_memory()`
3. Test with P=1, P=2, P=max to verify correctness
4. Update `PerformanceAnalyzer` if adding new metrics

### Platform-Specific Code
Use preprocessor directives:
```cpp
#ifdef _WIN32
    // Windows-specific code
#else
    // POSIX (Linux/macOS) code
#endif
```

## Important Implementation Details

**Block multiplication order:**
- Outer loops: iterate over blocks
- Inner loops: standard i-j-k multiplication within block
- This ordering maximizes cache hits (data reuse)

**Process work distribution:**
- Each process gets `total_blocks / num_processes` blocks
- Remainder blocks assigned to first few processes
- See `ProcessManager::calculate_block_distribution()`

**Shared memory naming:**
- Uses `/matrix_A_<random>`, `/matrix_B_<random>`, `/matrix_C_<random>`
- Names must start with `/` on POSIX systems
- Automatically cleaned up in destructor

**Performance optimization:**
- Block size calculated to fit 3 blocks (A, B, C) in L1 cache
- Sequential version also uses blocking (not naive triple loop)
- This means parallel speedup isolates parallelization benefits
