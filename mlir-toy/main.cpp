#include "mlir/IR/AsmState.h"
#include "mlir/IR/Builders.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/IR/MLIRContext.h"
#include "mlir/Parser/Parser.h"
#include "mlir/Support/FileUtilities.h"

#include "mlir/Dialect/Arith/IR/Arith.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "llvm/Support/raw_ostream.h"

using namespace mlir;

int main(int argc, char **argv) {
  MLIRContext ctx;
  ctx.loadDialect<func::FuncDialect, arith::ArithDialect>();

  // Create OpBuilder
  OpBuilder builder(&ctx);
  auto mod = builder.create<ModuleOp>(builder.getUnknownLoc());

  // Set insertion point
  builder.setInsertionPointToEnd(mod.getBody());

  // Create func
  auto i32 = builder.getI32Type();
  auto funcType = builder.getFunctionType({i32, i32}, {i32});
  auto func =
      builder.create<func::FuncOp>(builder.getUnknownLoc(), "test", funcType);

  // Add entry block
  auto entry = func.addEntryBlock();
  auto args = entry->getArguments();

  // Set insertion point
  builder.setInsertionPointToEnd(entry);

  // Create arith.addi
  auto addi =
      builder.create<arith::AddIOp>(builder.getUnknownLoc(), args[0], args[1]);

  // Create func.return
  builder.create<func::ReturnOp>(builder.getUnknownLoc(), ValueRange({addi}));
  mod->print(llvm::outs());
  return 0;
}