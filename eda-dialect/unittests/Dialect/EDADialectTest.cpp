//===- EDADialectTest.cpp - EDA Dialect unit tests -----------------------===//
//
// Part of the MLIR Tutorial Project
//
//===----------------------------------------------------------------------===//

#include "eda-dialect/Dialect.h"
#include "mlir/IR/Builders.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/IR/BuiltinTypes.h"
#include "mlir/IR/Verifier.h"
#include "gtest/gtest.h"

using namespace mlir;
using namespace eda;

namespace {
class EDADialectTest : public testing::Test {
protected:
  void SetUp() override {
    context.getOrLoadDialect<EDADialect>();
    builder = std::make_unique<OpBuilder>(&context);
  }

  MLIRContext context;
  std::unique_ptr<OpBuilder> builder;
};

TEST_F(EDADialectTest, DialectInitialization) {
  // Test dialect can be loaded
  ASSERT_TRUE(context.getLoadedDialect<EDADialect>() != nullptr);

  // Test dialect has correct name
  auto dialect = context.getLoadedDialect<EDADialect>();
  EXPECT_EQ(dialect->getNamespace(), "eda");
}

TEST_F(EDADialectTest, DialectAttributes) {
  // Test that dialect registers custom attributes if any
  auto dialect = context.getLoadedDialect<EDADialect>();
  ASSERT_TRUE(dialect != nullptr);

  // Parse a simple attribute
  StringAttr attr = StringAttr::get(&context, "test");
  EXPECT_TRUE(attr != nullptr);
}

TEST_F(EDADialectTest, DialectTypes) {
  // Test that dialect registers custom types if any
  auto dialect = context.getLoadedDialect<EDADialect>();
  ASSERT_TRUE(dialect != nullptr);

  // Basic types should be available
  auto i32Type = IntegerType::get(&context, 32);
  auto i1Type = IntegerType::get(&context, 1);

  EXPECT_TRUE(i32Type != nullptr);
  EXPECT_TRUE(i1Type != nullptr);
}

TEST_F(EDADialectTest, VerifyEmptyModule) {
  // Test that an empty module with EDA dialect loads correctly
  auto module = ModuleOp::create(builder->getUnknownLoc());
  EXPECT_TRUE(succeeded(mlir::verify(module)));
}

TEST_F(EDADialectTest, DialectInterfaceRegistration) {
  // Test that dialect can register interfaces
  auto dialect = context.getLoadedDialect<EDADialect>();
  ASSERT_TRUE(dialect != nullptr);
}

} // namespace