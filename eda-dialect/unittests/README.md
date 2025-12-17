# EDA Dialect Unit Tests

This directory contains the unit tests for the EDA Dialect implementation.

## Test Structure

### Organization

- **Dialect/** - Core dialect and type tests
  - `EDADialectTest.cpp` - Basic dialect initialization and attributes
  - `EDAOpsTest.cpp` - Operation creation and verification tests
  - `EDATypesTest.cpp` - Custom type system tests

- **Transformation/** - Pass and optimization tests
  - `EDAPassesTest.cpp` - Tests for constant folding and dead code elimination passes

- **Translation/** - Code generation tests
  - `EDATranslationTest.cpp` - MLIR to TCL translation tests

## Running Tests

### Using the Test Runner Script

The easiest way to run all tests is to use the provided test runner:

```bash
cd eda-dialect
./run_tests.sh
```

### Building and Running Tests Manually

1. **Build the project with tests**:
```bash
cd eda-dialect
mkdir build && cd build
cmake .. -G Ninja \
  -DMLIR_DIR=/mlir-tutorial/install/lib/cmake/mlir \
  -DLLVM_DIR=/mlir-tutorial/install/lib/cmake/llvm \
  -DCMAKE_BUILD_TYPE=Release
ninja
```

2. **Run all tests**:
```bash
./unittests/EDADialectTests
```

3. **Run specific test categories**:
```bash
# Run only dialect tests
./unittests/EDADialectTests --gtest_filter="EDADialect.*"

# Run only operation tests
./unittests/EDADialectTests --gtest_filter="EDAOp*"

# Run only pass tests
./unittests/EDADialectTests --gtest_filter="EDAPass*"
```

4. **Run with verbose output**:
```bash
./unittests/EDADialectTests --gtest_print_time=1 --gtest_output=xml
```

## Test Coverage

### Dialect Tests

- Dialect initialization and namespace verification
- Attribute registration and type checking
- Interface registration

### Operation Tests

- **Variable Operations**:
  - AssignOp: Variable assignment with type checking
  - LoadOp: Variable loading
  - StoreOp: Variable storing

- **Arithmetic Operations**:
  - AddOp, SubOp, MulOp, DivOp
  - Integer and floating-point support
  - Type compatibility

- **Comparison Operations**:
  - CmpEqOp, CmpGtOp, CmpLtOp
  - Boolean result type verification

- **Logical Operations**:
  - AndOp, OrOp, NotOp
  - Boolean operand requirements

- **System Operations**:
  - PutsOp: Message output
  - FormatOp: String formatting
  - ConcatOp: String concatenation

- **Constant Operations**:
  - Integer, float, string, and boolean constants
  - Type correctness verification

### Pass Tests

- **Constant Folding**:
  - Constant comparison folding
  - Known conditional optimization
  - Pattern application

- **Dead Code Elimination**:
  - Unused operation removal
  - Side effect preservation
  - Pass chaining

### Translation Tests

- Basic operation translation
- Module translation
- Function translation
- Complex expression translation

## Writing New Tests

### Test Naming Convention

- Test files: `[Feature]Test.cpp`
- Test fixtures: `[Feature]Test`
- Test cases: `[Scenario][ExpectedResult]`

### Test Structure Template

```cpp
#include "eda-dialect/Dialect.h"
#include "eda-dialect/Ops.h"
#include "mlir/IR/Builders.h"
#include "mlir/IR/BuiltinTypes.h"
#include "mlir/IR/Verifier.h"
#include "gtest/gtest.h"

using namespace mlir;
using namespace eda;

namespace {
class [Feature]Test : public testing::Test {
protected:
  void SetUp() override {
    context.getOrLoadDialect<EDADialect>();
    builder = std::make_unique<OpBuilder>(&context);
  }

  MLIRContext context;
  std::unique_ptr<OpBuilder> builder;
};

TEST_F([Feature]Test, [TestName]) {
  // Test implementation
  EXPECT_TRUE(condition);
}
} // namespace
```

### Best Practices

1. **Use test fixtures** for shared setup code
2. **Test both success and failure cases**
3. **Verify module validity** with `mlir::verify()`
4. **Test type correctness** for operations
5. **Check expected vs actual results**
6. **Use descriptive test names**

## Debugging Tests

### Running Tests with GDB

```bash
gdb --args ./unittests/EDADialectTests --gtest_filter="SpecificTest"
```

### Enable Debug Output

```bash
./unittests/EDADialectTests --gtest_print_time=1 --gtest_break_on_failure
```

### Common Issues

1. **MLIR not found**: Ensure MLIR is built and CMake can find it
2. **Missing symbols**: Check that all required libraries are linked
3. **Build failures**: Verify C++17 support and proper dependencies

## Integration Tests

In addition to unit tests, the project includes integration tests using the actual MLIR files in the `examples/` directory. The test runner script automatically tests the translation tool with all example files.

## Contributing

When adding new features to the EDA Dialect:

1. Add corresponding unit tests
2. Verify all tests pass with `./run_tests.sh`
3. Update this README if adding new test categories
4. Consider adding integration tests for complex features