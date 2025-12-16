# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is an MLIR (Multi-Level Intermediate Representation) tutorial project containing:
- **llvm-project**: A git submodule pointing to a fork of LLVM (cpu0 fork)
- **mlir-toy**: A simple example project demonstrating basic MLIR usage and API concepts

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

### 3. Run mlir-toy
```bash
cd mlir-toy/build
./mlir-toy
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

### MLIR Concepts Demonstrated
- Context and dialect management
- OpBuilder for programmatic MLIR construction
- Function creation with proper block structure
- Basic arithmetic operations
- Return operations

## Important Notes

- This is a tutorial project - keep changes simple and educational
- The build system is configured for cross-platform development (Linux and macOS)
- Use CMake with Ninja for efficient builds
- Install prefix is standardized to `/mlir-tutorial/install` for both LLVM and mlir-toy