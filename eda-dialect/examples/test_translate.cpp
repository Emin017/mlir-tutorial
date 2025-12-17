//===-- test_translate.cpp - Test MLIR to TCL translation ----------------===//
//
// This example demonstrates the MLIR to TCL translation capability
//

#include "eda-dialect/Dialect.h"
#include "eda-dialect/Transforms/TclCodeGen.h"
#include "mlir/IR/Builders.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/IR/MLIRContext.h"
#include "mlir/Support/LogicalResult.h"
#include "llvm/Support/raw_ostream.h"

using namespace mlir;
using namespace eda;

int main() {
  MLIRContext context;
  context.loadDialect<EDADialect>();

  auto loc = UnknownLoc::get(&context);

  // Create a module
  ModuleOp module = ModuleOp::create(loc);
  Block &moduleBody = *module.getBody();

  OpBuilder builder(&context);
  builder.setInsertionPoint(&moduleBody, moduleBody.end());

  // Create a simple assign operation: assign("timeout", "10")
  auto stringType = StringType::get(&context);

  // Create constant
  auto constOp = builder.create<ConstantOp>(
      loc, StringAttr::get(&context, "10"));

  // Create assign
  auto assignOp = builder.create<AssignOp>(
      loc, stringType, "timeout", constOp.getResult());

  // Create puts operation
  builder.create<PutsOp>(loc, assignOp.getResult());

  // Print the MLIR
  llvm::outs() << "=== Generated MLIR ===\n";
  module.print(llvm::outs());

  // Translate to TCL
  llvm::outs() << "\n=== Translated to TCL ===\n";
  if (failed(translateToTcl(module, llvm::outs()))) {
    llvm::errs() << "Translation failed\n";
    return 1;
  }

  return 0;
}
