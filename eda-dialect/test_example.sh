#!/bin/bash

# Test script for EDA Dialect
# This script demonstrates how to build and use the EDA Dialect

echo "=== EDA Dialect Test Script ==="
echo

# Check if we have the necessary tools
echo "1. Checking for required tools..."
if ! command -v cmake &> /dev/null; then
    echo "Error: cmake is not installed"
    exit 1
fi

if ! command -v ninja &> /dev/null; then
    echo "Error: ninja is not installed"
    exit 1
fi

echo "✓ Tools found"

# Set up build directory
echo
echo "2. Setting up build directory..."
mkdir -p build
cd build

# Configure the build
echo
echo "3. Configuring the build..."
# Note: This assumes LLVM/MLIR is installed in a standard location
# You may need to adjust the paths
cmake .. -G Ninja \
  -DMLIR_DIR=/mlir-tutorial/install/lib/cmake/mlir \
  -DLLVM_DIR=/mlir-tutorial/install/lib/cmake/llvm \
  -DCMAKE_BUILD_TYPE=Release

if [ $? -ne 0 ]; then
    echo "Error: CMake configuration failed"
    echo "Make sure LLVM/MLIR is built and installed in /mlir-tutorial/install"
    exit 1
fi

echo "✓ Build configured"

# Build the project
echo
echo "4. Building EDA Dialect..."
ninja

if [ $? -ne 0 ]; then
    echo "Error: Build failed"
    exit 1
fi

echo "✓ Build completed"

# Test with examples
echo
echo "5. Testing with examples..."

# List of examples to test
EXAMPLES=(
    "simple_example.mlir"
    "basic_ops.mlir"
    "control_flow.mlir"
    "function_example.mlir"
)

for example in "${EXAMPLES[@]}"; do
    echo
    echo "Processing: $example"
    if [ -f "../examples/$example" ]; then
        # Try to translate to TCL
        echo "  Translating to TCL..."
        ./mlir-eda-translate ../examples/$example -mlir-to-tcl > ${example%.mlir}.tcl 2>&1

        if [ $? -eq 0 ]; then
            echo "  ✓ Translation successful"
            echo "  Output:"
            head -20 ${example%.mlir}.tcl | sed 's/^/    /'
        else
            echo "  ✗ Translation failed"
            cat ${example%.mlir}.tcl
        fi
    else
        echo "  ✗ Example file not found"
    fi
done

echo
echo "6. Build summary:"
echo "  EDA Dialect tools built:"
echo "    - mlir-eda-opt: optimization tool"
echo "    - mlir-eda-translate: MLIR to TCL translator"
echo
echo "  Generated TCL files are in: build/*.tcl"

echo
echo "=== Test Complete ==="