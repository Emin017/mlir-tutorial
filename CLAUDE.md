# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is an MLIR (Multi-Level Intermediate Representation) tutorial project containing:
- **llvm-project**: A git submodule pointing to a fork of LLVM (cpu0 fork)
- **mlir-toy**: A simple example project demonstrating basic MLIR usage and API concepts
- **eda-dialect**: A comprehensive custom MLIR dialect for EDA (Electronic Design Automation) tool development

## Development Environment

The project uses **direnv** with Nix flakes. The `.envrc` file automatically loads the Nix development shell which provides:
- LLVM 18 development tools
- CMake and Ninja for building
- gdb for debugging
- Code formatting tools (clang-format, nixfmt)

## Build Commands

### 1. Build LLVM/MLIR
```bash
cd llvm-project
mkdir build && cd build
cmake -G Ninja ../llvm \
  -DCMAKE_INSTALL_PREFIX=/mlir-tutorial/install \
  -DLLVM_ENABLE_PROJECTS=mlir \
  -DLLVM_BUILD_EXAMPLES=ON \
  -DLLVM_TARGETS_TO_BUILD="Native;NVPTX;AMDGPU" \
  -DCMAKE_BUILD_TYPE=Release \
  -DLLVM_ENABLE_ASSERTIONS=ON -DLLVM_PARALLEL_LINK_JOBS=2 -DLLVM_RAM_PER_LINK_JOB=12000
ninja
```

### 2. Build mlir-toy
```bash
cd mlir-toy
mkdir build && cd build
cmake .. -GNinja -DCMAKE_INSTALL_PREFIX=/mlir-tutorial/install
ninja
```

### 3. Build eda-dialect
```bash
cd eda-dialect
mkdir build && cd build
cmake .. -G Ninja \
  -DMLIR_DIR=/mlir-tutorial/install/lib/cmake/mlir \
  -DLLVM_DIR=/mlir-tutorial/install/lib/cmake/llvm \
  -DCMAKE_BUILD_TYPE=Release
ninja
```

### 4. Run mlir-toy
```bash
cd mlir-toy/build
./mlir-toy
```

### 5. Test eda-dialect
```bash
cd eda-dialect
# Run all tests (builds if needed)
./run_tests.sh

# Or manually run unit tests
cd build
./unittests/EDASimpleTests

# Run specific test
./unittests/EDASimpleTests --gtest_filter=TestName

# Run with CTest
ctest --verbose
```

## Architecture

### mlir-toy Project
The main educational component demonstrating core MLIR concepts:
- **main.cpp**: Shows how to:
  - Create an MLIR context and load dialects (Func, Arith)
  - Use OpBuilder to construct MLIR operations programmatically
  - Build functions with blocks and arguments
  - Create basic arithmetic operations (arith.addi)
- **tests/dummy.mlir**: Example MLIR file showing function structure

### eda-dialect Project
A comprehensive custom MLIR dialect for EDA tool development:
- **Purpose**: Type-safe TCL code generation for EDA workflows
- **Key Components**:
  - **include/eda-dialect/**: TableGen definitions (.td files) for dialect, operations, and types
    - `Dialect.h`, `EDAOps.td`, `EDATypes.td`: Core dialect definitions
    - `Ops.h`, `Types.h`: Generated C++ headers for operations and types
  - **lib/Dialect/**: Implementation of dialect operations and types
  - **lib/Transformation/**: Transformation passes (e.g., DeadCodeElimination.cpp)
  - **unittests/**: Google Test-based unit tests
    - `Dialect/`: Tests for dialect operations and types
    - `Transformation/`: Tests for transformation passes
    - `Translation/`: Tests for code translation
  - **tools/** (currently disabled in CMakeLists.txt):
    - `mlir-eda-opt`: Optimization tool with custom passes
    - `mlir-eda-translate`: MLIR to TCL code generator
- **Testing Infrastructure**:
  - Uses Google Test framework
  - `run_tests.sh`: Automated test runner that builds and runs all tests
  - Supports CTest for running individual or filtered tests

### MLIR Concepts Demonstrated
- Context and dialect management
- OpBuilder for programmatic MLIR construction
- Function creation with proper block structure
- Basic arithmetic operations
- Return operations
- Custom dialect development with TableGen (.td files)
- Operation definition and implementation
- Type system design
- Transformation passes
- Code generation and translation
- Unit testing with Google Test

## Important Notes

- This is a tutorial project - keep changes simple and educational
- The build system is configured for cross-platform development (Linux and macOS)
- Use CMake with Ninja for efficient builds
- Install prefix is standardized to `/mlir-tutorial/install` for both LLVM and mlir-toy
- LLVM/MLIR must be built and installed before building other components
- The project uses direnv with Nix flakes for reproducible development environments
- eda-dialect requires Google Test for unit testing
- Tools directory (mlir-eda-opt, mlir-eda-translate) is currently disabled in CMakeLists.txt

## Development Workflow

1. Ensure direnv is enabled and loads the Nix development shell automatically
2. Build LLVM/MLIR first with the specified configuration
3. Build individual components (mlir-toy, eda-dialect)
4. Use mlir-toy to understand basic MLIR concepts
5. Study eda-dialect for advanced custom dialect development
6. Use provided tools for optimization and code generation