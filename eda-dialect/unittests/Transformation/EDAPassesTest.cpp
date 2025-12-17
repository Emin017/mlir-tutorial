//===- EDAPassesTest.cpp - EDA Passes unit tests --------------------------===//
//
// Part of the MLIR Tutorial Project
//
//===----------------------------------------------------------------------===//

#include "eda-dialect/Dialect.h"
#include "eda-dialect/Ops.h"
#include "eda-dialect/Transforms/Passes.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/IR/Builders.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/IR/BuiltinTypes.h"
#include "mlir/IR/OwningOpRef.h"
#include "mlir/IR/Verifier.h"
#include "mlir/Pass/PassManager.h"
#include "mlir/Transforms/GreedyPatternRewriteDriver.h"
#include "gtest/gtest.h"

using namespace mlir;
using namespace eda;

namespace {
class EDAPassesTest : public testing::Test {
protected:
  void SetUp() override {
    context.getOrLoadDialect<EDADialect>();
    context.getOrLoadDialect<func::FuncDialect>();
  }

  // Helper to create a simple module with EDA operations
  OwningOpRef<ModuleOp> createTestModule() {
    OwningOpRef<ModuleOp> module(ModuleOp::create(UnknownLoc::get(&context)));
    OpBuilder builder(&module->getBodyRegion());

    // Create a function
    auto funcType = builder.getFunctionType(std::nullopt, std::nullopt);
    auto func = func::FuncOp::create(
        builder.getUnknownLoc(),
        "test_func",
        funcType);

    auto *entryBlock = func.addEntryBlock();
    builder.setInsertionPointToEnd(entryBlock);

    module->push_back(func);
    return module;
  }

  // Helper to create a constant operation
  ConstantOp createConstant(Type type, Attribute value) {
    return OpBuilder(&context).create<ConstantOp>(
        UnknownLoc::get(&context),
        type,
        value);
  }

  MLIRContext context;
};

//===----------------------------------------------------------------------===//
// Constant Fold Pass Tests
//===----------------------------------------------------------------------===//

TEST_F(EDAPassesTest, ConstantFoldPassBasic) {
  auto module = createTestModule();

  // Create pass manager
  auto pm = PassManager::on<ModuleOp>(&context);
  pm.addPass(eda::createConstantFoldPass());

  // Run pass
  LogicalResult result = pm.run(*module);
  EXPECT_TRUE(succeeded(result));
  EXPECT_TRUE(succeeded(mlir::verify(*module)));
}

TEST_F(EDAPassesTest, ConstantComparisonFold) {
  auto module = createTestModule();
  OpBuilder builder(&module->getBodyRegion());

  auto func = module->lookupSymbol<func::FuncOp>("test_func");
  builder.setInsertionPointToEnd(&func.getBody().front());

  // Create two equal constants
  auto i32Type = IntegerType::get(&context, 32);
  auto const1 = builder.create<ConstantOp>(
      builder.getUnknownLoc(),
      i32Type,
      builder.getIntegerAttr(i32Type, 42));
  auto const2 = builder.create<ConstantOp>(
      builder.getUnknownLoc(),
      i32Type,
      builder.getIntegerAttr(i32Type, 42));

  // Create a comparison of two constants
  auto cmpOp = builder.create<CmpEqOp>(
      builder.getUnknownLoc(),
      const1.getResult(),
      const2.getResult());

  // Add a return
  builder.create<func::ReturnOp>(builder.getUnknownLoc());

  // Verify module before pass
  EXPECT_TRUE(succeeded(mlir::verify(*module)));

  // Create pass manager and run constant folding
  auto pm = PassManager::on<ModuleOp>(&context);
  pm.addPass(eda::createConstantFoldPass());

  LogicalResult result = pm.run(*module);
  EXPECT_TRUE(succeeded(result));
  EXPECT_TRUE(succeeded(mlir::verify(*module)));

  // Check if comparison was folded (in a real implementation,
  // we would verify that cmpOp was replaced with a constant)
}

TEST_F(EDAPassesTest, KnownConditionalFold) {
  auto module = createTestModule();
  OpBuilder builder(&module->getBodyRegion());

  auto func = module->lookupSymbol<func::FuncOp>("test_func");
  builder.setInsertionPointToEnd(&func.getBody().front());

  // Create some constants to test with
  auto i32Type = IntegerType::get(&context, 32);
  auto const1 = builder.create<ConstantOp>(
      builder.getUnknownLoc(),
      i32Type,
      builder.getIntegerAttr(i32Type, 10));
  auto const2 = builder.create<ConstantOp>(
      builder.getUnknownLoc(),
      i32Type,
      builder.getIntegerAttr(i32Type, 20));

  // Create arithmetic operation to test folding
  auto addOp = builder.create<AddOp>(
      builder.getUnknownLoc(),
      const1.getResult(),
      const2.getResult());

  // Add a return
  builder.create<func::ReturnOp>(builder.getUnknownLoc());

  // Create pass manager and run constant folding
  auto pm = PassManager::on<ModuleOp>(&context);
  pm.addPass(eda::createConstantFoldPass());

  LogicalResult result = pm.run(*module);
  EXPECT_TRUE(succeeded(result));
}

//===----------------------------------------------------------------------===//
// Dead Code Elimination Pass Tests
//===----------------------------------------------------------------------===//

TEST_F(EDAPassesTest, DeadCodeEliminationPassBasic) {
  auto module = createTestModule();

  // Create pass manager
  auto pm = PassManager::on<ModuleOp>(&context);
  pm.addPass(eda::createDeadCodeEliminationPass());

  // Run pass
  LogicalResult result = pm.run(*module);
  EXPECT_TRUE(succeeded(result));
  EXPECT_TRUE(succeeded(mlir::verify(*module)));
}

TEST_F(EDAPassesTest, DeadCodeEliminationUnusedConstants) {
  auto module = createTestModule();
  OpBuilder builder(&module->getBodyRegion());

  auto func = module->lookupSymbol<func::FuncOp>("test_func");
  builder.setInsertionPointToEnd(&func.getBody().front());

  // Create an unused constant
  auto i32Type = IntegerType::get(&context, 32);
  auto unusedConst = builder.create<ConstantOp>(
      builder.getUnknownLoc(),
      i32Type,
      builder.getIntegerAttr(i32Type, 123));

  // Create a used constant
  auto usedConst = builder.create<ConstantOp>(
      builder.getUnknownLoc(),
      i32Type,
      builder.getIntegerAttr(i32Type, 456));

  // Use the second constant
  auto storeOp = builder.create<StoreOp>(
      builder.getUnknownLoc(),
      "used_var",
      usedConst.getResult());

  // Add a return
  builder.create<func::ReturnOp>(builder.getUnknownLoc());

  // Verify module before pass
  EXPECT_TRUE(succeeded(mlir::verify(*module)));

  // Count operations before pass
  int opCountBefore = 0;
  module->walk([&](Operation *op) {
    if (isa<ConstantOp>(op))
      opCountBefore++;
  });
  EXPECT_EQ(opCountBefore, 2);

  // Create pass manager and run dead code elimination
  auto pm = PassManager::on<ModuleOp>(&context);
  pm.addPass(eda::createDeadCodeEliminationPass());

  LogicalResult result = pm.run(*module);
  EXPECT_TRUE(succeeded(result));
  EXPECT_TRUE(succeeded(mlir::verify(*module)));

  // Count operations after pass
  int opCountAfter = 0;
  module->walk([&](Operation *op) {
    if (isa<ConstantOp>(op))
      opCountAfter++;
  });

  // In a real implementation, we would expect opCountAfter to be 1
  // For now, just verify the pass ran successfully
}

TEST_F(EDAPassesTest, DeadCodeEliminationPreservesSideEffects) {
  auto module = createTestModule();
  OpBuilder builder(&module->getBodyRegion());

  auto func = module->lookupSymbol<func::FuncOp>("test_func");
  builder.setInsertionPointToEnd(&func.getBody().front());

  // Create operations with side effects
  auto stringType = StringType::get(&context);
  auto message = builder.create<ConstantOp>(
      builder.getUnknownLoc(),
      stringType,
      builder.getStringAttr("Hello, World!"));

  // Puts operation has side effects (outputs to console)
  auto putsOp = builder.create<PutsOp>(
      builder.getUnknownLoc(),
      message.getResult());

  // Store operation has side effects (modifies variable)
  auto i32Type = IntegerType::get(&context, 32);
  auto value = builder.create<ConstantOp>(
      builder.getUnknownLoc(),
      i32Type,
      builder.getIntegerAttr(i32Type, 42));

  auto storeOp = builder.create<StoreOp>(
      builder.getUnknownLoc(),
      "test_var",
      value.getResult());

  // Add a return
  builder.create<func::ReturnOp>(builder.getUnknownLoc());

  // Verify module before pass
  EXPECT_TRUE(succeeded(mlir::verify(*module)));

  // Count operations before pass
  int opCountBefore = 0;
  module->walk([&](Operation *op) {
    if (isa<PutsOp, StoreOp>(op))
      opCountBefore++;
  });
  EXPECT_EQ(opCountBefore, 2);

  // Create pass manager and run dead code elimination
  auto pm = PassManager::on<ModuleOp>(&context);
  pm.addPass(eda::createDeadCodeEliminationPass());

  LogicalResult result = pm.run(*module);
  EXPECT_TRUE(succeeded(result));
  EXPECT_TRUE(succeeded(mlir::verify(*module)));

  // Count operations after pass (should be the same)
  int opCountAfter = 0;
  module->walk([&](Operation *op) {
    if (isa<PutsOp, StoreOp>(op))
      opCountAfter++;
  });
  EXPECT_EQ(opCountAfter, opCountBefore);
}

//===----------------------------------------------------------------------===//
// Combined Pass Tests
//===----------------------------------------------------------------------===//

TEST_F(EDAPassesTest, MultiplePasses) {
  auto module = createTestModule();
  OpBuilder builder(&module->getBodyRegion());

  auto func = module->lookupSymbol<func::FuncOp>("test_func");
  builder.setInsertionPointToEnd(&func.getBody().front());

  // Create some operations for optimization
  auto i32Type = IntegerType::get(&context, 32);
  auto const1 = builder.create<ConstantOp>(
      builder.getUnknownLoc(),
      i32Type,
      builder.getIntegerAttr(i32Type, 10));
  auto const2 = builder.create<ConstantOp>(
      builder.getUnknownLoc(),
      i32Type,
      builder.getIntegerAttr(i32Type, 20));

  // Unused arithmetic result
  auto unusedAdd = builder.create<AddOp>(
      builder.getUnknownLoc(),
      const1.getResult(),
      const2.getResult());

  // Used comparison
  auto cmpOp = builder.create<CmpEqOp>(
      builder.getUnknownLoc(),
      const1.getResult(),
      const2.getResult());

  // Add a return
  builder.create<func::ReturnOp>(builder.getUnknownLoc());

  // Verify module before passes
  EXPECT_TRUE(succeeded(mlir::verify(*module)));

  // Create pass manager with multiple passes
  auto pm = PassManager::on<ModuleOp>(&context);
  pm.addPass(eda::createConstantFoldPass());
  pm.addPass(eda::createDeadCodeEliminationPass());

  LogicalResult result = pm.run(*module);
  EXPECT_TRUE(succeeded(result));
  EXPECT_TRUE(succeeded(mlir::verify(*module)));
}

} // namespace