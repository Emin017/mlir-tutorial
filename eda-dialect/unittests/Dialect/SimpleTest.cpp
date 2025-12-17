//===- SimpleTest.cpp - Simple EDA Dialect Test --------------------------===//
//
// Part of the MLIR Tutorial Project
//
//===----------------------------------------------------------------------===//

#include "mlir/IR/Builders.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/IR/BuiltinTypes.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/IR/Dialect.h"
#include "mlir/IR/MLIRContext.h"
#include "mlir/IR/Verifier.h"
#include "gtest/gtest.h"

using namespace mlir;

namespace {

// Simple test class
class EDASimpleTest : public testing::Test {
protected:
  void SetUp() override {
    // Create a simple context for testing
    context = std::make_unique<MLIRContext>();
  }

  std::unique_ptr<MLIRContext> context;
};

TEST_F(EDASimpleTest, BasicContextTest) {
  // Test that we can create a basic MLIR context
  ASSERT_TRUE(context != nullptr);

  // Test basic types
  auto i32Type = IntegerType::get(context.get(), 32);
  auto i1Type = IntegerType::get(context.get(), 1);
  auto f32Type = Float32Type::get(context.get());

  EXPECT_TRUE(i32Type != nullptr);
  EXPECT_TRUE(i1Type != nullptr);
  EXPECT_TRUE(f32Type != nullptr);
}

TEST_F(EDASimpleTest, BasicModuleTest) {
  // Create a simple module
  auto module = ModuleOp::create(UnknownLoc::get(context.get()));
  EXPECT_TRUE(module != nullptr);
  EXPECT_TRUE(succeeded(mlir::verify(module)));
}

TEST_F(EDASimpleTest, BasicOperationsTest) {
  // Create a module and builder
  auto module = ModuleOp::create(UnknownLoc::get(context.get()));
  OpBuilder builder(&module.getBodyRegion());

  // Test that we can create a function type
  auto funcType = builder.getFunctionType({}, {});
  EXPECT_TRUE(funcType != nullptr);

  // Test that we can count operations
  int opCount = 0;
  module.walk([&](Operation *op) {
    opCount++;
  });

  EXPECT_GT(opCount, 0);
}

} // namespace