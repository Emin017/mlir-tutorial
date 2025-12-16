# Building and Testing EDA Dialect

This guide explains how to build and test the EDA Dialect implementation.

## Prerequisites

1. **LLVM/MLIR 18+**: You must have LLVM and MLIR built and installed.
2. **CMake 3.15.4+**: For building the project.
3. **Ninja**: For faster builds (recommended).
4. **C++17 compatible compiler**: GCC 7+, Clang 5+, or MSVC 2019+.

## Step 1: Build LLVM/MLIR

If you haven't already built LLVM/MLIR:

```bash
cd llvm-project
mkdir build && cd build
cmake -G Ninja ../llvm \
  -DCMAKE_INSTALL_PREFIX=/mlir-tutorial/install \
  -DLLVM_ENABLE_PROJECTS=mlir \
  -DLLVM_TARGETS_TO_BUILD="Native" \
  -DCMAKE_BUILD_TYPE=Release \
  -DLLVM_ENABLE_ASSERTIONS=ON
ninja install
```

## Step 2: Build EDA Dialect

```bash
cd eda-dialect
mkdir build && cd build
cmake .. -G Ninja \
  -DMLIR_DIR=/mlir-tutorial/install/lib/cmake/mlir \
  -DLLVM_DIR=/mlir-tutorial/install/lib/cmake/llvm \
  -DCMAKE_BUILD_TYPE=Release
ninja
```

## Step 3: Test the Implementation

### 3.1 Test Basic Operations

```bash
# Translate a simple example to TCL
./mlir-eda-translate ../examples/simple_example.mlir -mlir-to-tcl

# Expected output should be TCL code
```

### 3.2 Run Optimization Passes

```bash
# Apply optimizations
./mlir-eda-opt ../examples/optimization_example.mlir \
  -eda-constant-fold \
  -eda-dead-code-elimination \
  > optimized.mlir

# View the optimized IR
cat optimized.mlir

# Translate optimized code
./mlir-eda-translate optimized.mlir -mlir-to-tcl > optimized.tcl
```

### 3.3 Test All Examples

```bash
# Run the test script
./test_example.sh
```

## Step 4: Using in Your Own Projects

### 4.1 Include the Dialect

In your C++ code:

```cpp
#include "eda-dialect/Dialect.h"
#include "eda-dialect/Ops.h"

// Register the dialect
mlir::DialectRegistry registry;
registry.insert<eda::EDADialect>();
mlir::MLIRContext context(registry);
```

### 4.2 Parse MLIR with EDA Operations

```cpp
// Parse a module
auto module = mlir::parseSourceString<mlir::ModuleOp>(mlirSource, &context);

// Walk through operations
module->walk([](eda::AssignOp assign) {
  llvm::outs() << "Found assignment to " << assign.getName() << "\n";
});
```

### 4.3 Optimize the Code

```cpp
// Create a pass manager
mlir::PassManager pm(&context);
pm.addPass(eda::createConstantFoldPass());
pm.addPass(eda::createDeadCodeEliminationPass());

// Run passes
if (mlir::failed(pm.run(*module))) {
  llvm::errs() << "Failed to run passes\n";
}
```

## Troubleshooting

### Common Issues

1. **CMake can't find MLIR**
   - Ensure MLIR is installed with `CMAKE_INSTALL_PREFIX`
   - Check that `MLIR_DIR` and `LLVM_DIR` are correct

2. **Build fails with undefined symbols**
   - Verify MLIR was built with `-DLLVM_BUILD_EXAMPLES=ON`
   - Make sure you're using the same compiler for both LLVM and EDA Dialect

3. **Translation produces empty output**
   - Check that the input MLIR file is valid
   - Ensure all operations are properly defined

### Debug Tips

1. **Enable debug output**:
   ```bash
   ./mlir-eda-translate input.mlir -mlir-to-tcl 2>&1 | tee debug.log
   ```

2. **Verify MLIR input**:
   ```bash
   # Use mlir-opt to validate
   mlir-opt --verify-diagnostics input.mlir
   ```

3. **Check generated code**:
   - Look at the generated .cpp.inc files in build/
   - Ensure TableGen generated the expected operations

## Next Steps

1. **Extend the Dialect**: Add more operations as needed
2. **Improve Optimizations**: Implement more sophisticated passes
3. **Add Frontend**: Create a Scala3 to MLIR translator
4. **Testing**: Add comprehensive unit tests

## Resources

- [MLIR Documentation](https://mlir.llvm.org/docs/)
- [TableGen Documentation](https://llvm.org/docs/TableGen/)
- [LLVM Programmer's Manual](https://llvm.org/docs/ProgrammersManual.html)