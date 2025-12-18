//===- test_tcl_generation.cpp - Test TCL code generation ----------------===//

#include "eda-dialect/Dialect.h"
#include "eda-dialect/Ops.h"
#include "eda-dialect/Transforms/TclCodeGen.h"
#include "mlir/IR/Builders.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/IR/OwningOpRef.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "llvm/Support/raw_ostream.h"
#include <iostream>

using namespace mlir;
using namespace eda;

int main() {
  MLIRContext context;
  context.getOrLoadDialect<EDADialect>();
  context.getOrLoadDialect<func::FuncDialect>();

  // Create a simple module with operations
  OwningOpRef<ModuleOp> module(ModuleOp::create(UnknownLoc::get(&context)));
  OpBuilder builder(&module->getBodyRegion());

  // Create constant
  auto stringType = StringType::get(&context);
  auto constOp = builder.create<ConstantOp>(
      builder.getUnknownLoc(),
      stringType,
      builder.getStringAttr("Hello, World!"));

  // Create assign
  auto assignOp = builder.create<AssignOp>(
      builder.getUnknownLoc(),
      "greeting",
      constOp.getResult());

  // Create puts
  auto putsOp = builder.create<PutsOp>(
      builder.getUnknownLoc(),
      assignOp.getResult());

  // Translate to TCL
  std::string output;
  llvm::raw_string_ostream stream(output);

  LogicalResult result = eda::translateToTcl(*module, stream);

  if (succeeded(result)) {
    std::cout << "=== Generated TCL Code ===\n";
    std::cout << output;
    std::cout << "=== End TCL Code ===\n";
    return 0;
  } else {
    std::cerr << "Translation failed!\n";
    return 1;
  }
}
