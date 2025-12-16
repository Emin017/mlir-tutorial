//===-- DeadCodeElimination.cpp - EDA Dialect Dead Code Elimination Pass ===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "eda-dialect/Ops.h"
#include "mlir/IR/Dominance.h"
#include "mlir/IR/IRMapping.h"
#include "mlir/Interfaces/ControlFlowInterfaces.h"
#include "mlir/Pass/Pass.h"
#include "mlir/Transforms/Utils.h"

using namespace mlir;

namespace eda {

namespace {

/// This pass implements dead code elimination for the EDA dialect.
/// It removes operations whose results are not used.
class DeadCodeEliminationPass
    : public PassWrapper<DeadCodeEliminationPass, OperationPass<ModuleOp>> {
public:
  MLIR_DEFINE_EXPLICIT_INTERNAL_INLINE_TYPE_ID(DeadCodeEliminationPass)

  StringRef getArgument() const override { return "eda-dead-code-elimination"; }
  StringRef getDescription() const override {
    return "Eliminate dead code in the EDA dialect";
  }

  void runOnOperation() override {
    ModuleOp module = getOperation();

    // Walk through all operations in the module
    module.walk([this](Operation *op) {
      // Skip operations that produce no results
      if (op->getNumResults() == 0)
        return;

      // Check if all results are dead
      bool allDead = true;
      for (Value result : op->getResults()) {
        // If the result has users, it's not dead
        if (!result.use_empty()) {
          allDead = false;
          break;
        }
      }

      // If all results are dead and the operation can be safely removed,
      // erase it
      if (allDead && isOpTriviallyDead(op)) {
        op->erase();
      }
    });
  }

private:
  /// Check if an operation can be safely removed without side effects
  bool isOpTriviallyDead(Operation *op) {
    // Skip operations with side effects
    if (op->hasTrait<OpTrait::HasSideEffects>())
      return false;

    // Skip terminator operations
    if (op->hasTrait<OpTrait::IsTerminator>())
      return false;

    // Skip function operations
    if (isa<FuncOp>(op))
      return false;

    // Skip control flow operations
    if (isa<IfOp>(op) || isa<ForOp>(op) || isa<WhileOp>(op))
      return false;

    // Store operations have side effects (they modify variables)
    if (isa<StoreOp>(op))
      return false;

    // Puts operations have side effects (they output to console)
    if (isa<PutsOp>(op))
      return false;

    // File operations have side effects
    if (isa<FileReadOp>(op) || isa<FileWriteOp>(op))
      return false;

    // Everything else should be safe to remove if not used
    return true;
  }
};

} // namespace

std::unique_ptr<Pass> createDeadCodeEliminationPass() {
  return std::make_unique<DeadCodeEliminationPass>();
}

} // namespace eda