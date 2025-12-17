#!/bin/bash

# EDA Dialect Test Runner
# This script helps build and run the EDA Dialect tests

set -e  # Exit on any error

echo "=== EDA Dialect Test Runner ==="
echo

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Function to print colored output
print_status() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Check if we're in the right directory
if [ ! -f "CMakeLists.txt" ]; then
    print_error "CMakeLists.txt not found. Please run this script from the eda-dialect directory."
    exit 1
fi

# Create build directory if it doesn't exist
print_status "Setting up build directory..."
mkdir -p build
cd build

# Check if already configured
if [ ! -f "Makefile" ] && [ ! -f "build.ninja" ]; then
    print_status "Configuring build with CMake..."

    # Try to find MLIR/LLVM
    if [ -d "/mlir-tutorial/install" ]; then
        print_status "Using MLIR from /mlir-tutorial/install"
        cmake .. -G Ninja \
            -DMLIR_DIR=/mlir-tutorial/install/lib/cmake/mlir \
            -DLLVM_DIR=/mlir-tutorial/install/lib/cmake/llvm \
            -DCMAKE_BUILD_TYPE=Release \
            -DMLIR_ENABLE_TESTS=ON
    else
        print_warning "MLIR not found in /mlir-tutorial/install"
        print_status "Trying system MLIR..."
        cmake .. -G Ninja \
            -DCMAKE_BUILD_TYPE=Release \
            -DMLIR_ENABLE_TESTS=ON
    fi
fi

# Build the project
print_status "Building EDA Dialect and tests..."
ninja

# Check if tests were built
if [ ! -f "unittests/EDASimpleTests" ]; then
    print_error "Tests not found after build. Check build configuration."
    exit 1
fi

print_status "Running EDA Dialect unit tests..."
echo

# Run the tests
if ./unittests/EDASimpleTests; then
    print_status "All tests passed! ✓"
else
    print_error "Some tests failed!"
    exit 1
fi

echo
print_status "Running integration tests with examples..."

# Test translation tool
if [ -f "tools/mlir-eda-translate" ]; then
    echo
    print_status "Testing mlir-eda-translate with examples..."

    for example in ../examples/*.mlir; do
        if [ -f "$example" ]; then
            echo "Testing: $(basename "$example")"
            ./tools/mlir-eda-translate "$example" -mlir-to-tcl > /dev/null 2>&1
            if [ $? -eq 0 ]; then
                echo "  ✓ Translation successful"
            else
                echo "  ✗ Translation failed"
            fi
        fi
    done
else
    print_warning "mlir-eda-translate tool not found"
fi

echo
print_status "=== Test Summary ==="
echo "  - Unit tests: Completed"
echo "  - Integration tests: Completed"
echo "  - Tools: Available in build/tools/"
echo
print_status "To run individual tests:"
echo "  ./unittests/EDASimpleTests --gtest_filter=TestName"
echo
print_status "To run tests with CTest:"
echo "  ctest --verbose"