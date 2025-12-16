//===-- ConstantFold.cpp - EDA Dialect Constant Folding Pass ------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "eda-dialect/Ops.h"
#include "mlir/IR/Builders.h"
#include "mlir/IR/BuiltinTypes.h"
#include "mlir/IR/DialectRegistry.h"
#include "mlir/IR/PatternMatch.h"
#include "mlir/Pass/Pass.h"
#include "mlir/Transforms/GreedyPatternRewriteDriver.h"

using namespace mlir;

namespace eda {

namespace {

// Pattern to fold constant arithmetic operations
struct ConstantArithmeticFold : public OpRewritePattern<ConstantOp> {
  explicit ConstantArithmeticFold(MLIRContext *context)
      : OpRewritePattern<ConstantOp>(context, /*benefit=*/1) {}

  LogicalResult matchAndRewrite(ConstantOp op,
                                PatternRewriter &rewriter) const override {
    // This is a placeholder for constant folding
    // In a real implementation, we would analyze operations
    // that use this constant and fold if possible
    return failure();
  }
};

// Pattern to fold constant comparisons
struct ConstantComparisonFold : public OpRewritePattern<CmpEqOp> {
  explicit ConstantComparisonFold(MLIRContext *context)
      : OpRewritePattern<CmpEqOp>(context, /*benefit=*/1) {}

  LogicalResult matchAndRewrite(CmpEqOp op,
                                PatternRewriter &rewriter) const override {
    // Check if both operands are constants
    auto constLhs = op.getLhs().getDefiningOp<ConstantOp>();
    auto constRhs = op.getRhs().getDefiningOp<ConstantOp>();

    if (!constLhs || !constRhs)
      return failure();

    // Get the values
    Attribute lhsAttr = constLhs.getValue();
    Attribute rhsAttr = constRhs.getValue();

    // Compare the attributes
    bool isEqual = lhsAttr == rhsAttr;

    // Create a constant for the result
    auto boolType = IntegerType::get(getContext(), 1);
    auto result = rewriter.create<ConstantOp>(
        op.getLoc(),
        rewriter.getIntegerAttr(boolType, isEqual ? 1 : 0));

    // Replace the comparison with the constant
    rewriter.replaceOp(op, result.getResult());
    return success();
  }
};

// Pattern to fold known conditionals
struct KnownConditionalFold : public OpRewritePattern<IfOp> {
  explicit KnownConditionalFold(MLIRContext *context)
      : OpRewritePattern<IfOp>(context, /*benefit=*/2) {}

  LogicalResult matchAndRewrite(IfOp op,
                                PatternRewriter &rewriter) const override {
    // Check if the condition is a constant
    auto constCond = op.getCondition().getDefiningOp<ConstantOp>();
    if (!constCond)
      return failure();

    // Get the boolean value
    auto boolAttr = constCond.getValue().dyn_cast<IntegerAttr>();
    if (!boolAttr || !boolAttr.getType().isInteger(1))
      return failure();

    bool isTrue = boolAttr.getInt() != 0;

    // If the condition is always true and there's no else region,
    // we can inline the then region
    if (isTrue && op.getElseRegion().empty()) {
      // Move operations from then region to parent
      Block &thenBlock = op.getThenRegion().front();
      rewriter.inlineBlockBefore(&thenBlock, op.getOperation(),
                                 op.getOperands());

      // Remove the if operation
      rewriter.eraseOp(op);
      return success();
    }

    // If the condition is always false and we have an else region
    if (!isTrue && !op.getElseRegion().empty()) {
      // Move operations from else region to parent
      Block &elseBlock = op.getElseRegion().front();
      rewriter.inlineBlockBefore(&elseBlock, op.getOperation(),
                                 op.getOperands());

      // Remove the if operation
      rewriter.eraseOp(op);
      return success();
    }

    return failure();
  }
};

/// This pass implements constant folding for EDA operations.
class ConstantFoldPass
    : public PassWrapper<ConstantFoldPass, OperationPass<ModuleOp>> {
public:
  MLIR_DEFINE_EXPLICIT_INTERNAL_INLINE_TYPE_ID(ConstantFoldPass)

  StringRef getArgument() const override { return "eda-constant-fold"; }
  StringRef getDescription() const override {
    return "Fold constant operations in the EDA dialect";
  }

  void runOnOperation() override {
    RewritePatternSet patterns(&getContext());

    // Add folding patterns
    patterns.add<ConstantArithmeticFold,
                ConstantComparisonFold,
                KnownConditionalFold>(&getContext());

    // Apply the patterns
    if (failed(applyPatternsAndFoldGreedily(getOperation(),
                                           std::move(patterns)))) {
      signalPassFailure();
    }
  }
};

} // namespace

std::unique_ptr<Pass> createConstantFoldPass() {
  return std::make_unique<ConstantFoldPass>();
}

} // namespace eda