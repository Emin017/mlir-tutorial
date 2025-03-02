#include "mlir/IR/AsmState.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/IR/MLIRContext.h"
#include "mlir/Parser/Parser.h"
#include "mlir/Support/FileUtilities.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/Dialect/Arith/IR/Arith.h"
#include "llvm/Support/raw_ostream.h"

using namespace mlir;

int main(int argc, char ** argv) {
  MLIRContext ctx;
  // First, we register the dialects we want to use
  ctx.loadDialect<func::FuncDialect, arith::ArithDialect>();
  // Then, we parse the input file
  auto src = parseSourceFile<ModuleOp>(argv[1], &ctx);
  // Output the dialect to llvm::outs(), but we can also output to llvm::errs(), llvm::dbgs()
  src->print(llvm::outs());
  // Simple output, often used in debug
  src->dump();
  return 0;
}