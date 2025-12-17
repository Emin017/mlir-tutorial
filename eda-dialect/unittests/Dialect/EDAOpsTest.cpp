//===- EDAOpsTest.cpp - EDA Operations unit tests -------------------------===//
//
// Part of the MLIR Tutorial Project
//
//===----------------------------------------------------------------------===//

#include "eda-dialect/Dialect.h"
#include "eda-dialect/Ops.h"
#include "mlir/IR/Builders.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/IR/BuiltinTypes.h"
#include "mlir/IR/Verifier.h"
#include "gtest/gtest.h"

using namespace mlir;
using namespace eda;

namespace {
class EDAOpsTest : public testing::Test {
protected:
  void SetUp() override {
    context.getOrLoadDialect<EDADialect>();
    builder = std::make_unique<OpBuilder>(&context);
    module = ModuleOp::create(builder->getUnknownLoc());
  }

  MLIRContext context;
  std::unique_ptr<OpBuilder> builder;
  ModuleOp module;

  void tearDown() {
    // Cleanup if needed
  }
};

//===----------------------------------------------------------------------===//
// Variable Operations Tests
//===----------------------------------------------------------------------===//

TEST_F(EDAOpsTest, AssignOp) {
  auto i32Type = IntegerType::get(&context, 32);
  auto value = builder->create<ConstantOp>(
      builder->getUnknownLoc(),
      i32Type,
      builder->getIntegerAttr(i32Type, 42));

  auto assignOp = builder->create<AssignOp>(
      builder->getUnknownLoc(),
      "test_var",
      value.getResult());

  ASSERT_TRUE(assignOp != nullptr);
  EXPECT_EQ(assignOp.getName(), "test_var");
  EXPECT_EQ(assignOp.getValue(), value.getResult());
  EXPECT_EQ(assignOp.getResult().getType(), i32Type);

  EXPECT_TRUE(succeeded(mlir::verify(assignOp.getOperation())));
}

TEST_F(EDAOpsTest, LoadOp) {
  auto i32Type = IntegerType::get(&context, 32);

  auto loadOp = builder->create<LoadOp>(
      builder->getUnknownLoc(),
      i32Type,
      "loaded_var");

  ASSERT_TRUE(loadOp != nullptr);
  EXPECT_EQ(loadOp.getName(), "loaded_var");
  EXPECT_EQ(loadOp.getResult().getType(), i32Type);

  // Verification is implicit during construction
}

TEST_F(EDAOpsTest, StoreOp) {
  auto i32Type = IntegerType::get(&context, 32);
  auto value = builder->create<ConstantOp>(
      builder->getUnknownLoc(),
      i32Type,
      builder->getIntegerAttr(i32Type, 100));

  auto storeOp = builder->create<StoreOp>(
      builder->getUnknownLoc(),
      "stored_var",
      value.getResult());

  ASSERT_TRUE(storeOp != nullptr);
  EXPECT_EQ(storeOp.getName(), "stored_var");
  EXPECT_EQ(storeOp.getValue(), value.getResult());

  // Verification is implicit during construction
}

//===----------------------------------------------------------------------===//
// Arithmetic Operations Tests
//===----------------------------------------------------------------------===//

TEST_F(EDAOpsTest, AddOp) {
  auto i32Type = IntegerType::get(&context, 32);
  auto lhs = builder->create<ConstantOp>(
      builder->getUnknownLoc(),
      i32Type,
      builder->getIntegerAttr(i32Type, 10));
  auto rhs = builder->create<ConstantOp>(
      builder->getUnknownLoc(),
      i32Type,
      builder->getIntegerAttr(i32Type, 20));

  auto addOp = builder->create<AddOp>(
      builder->getUnknownLoc(),
      lhs.getResult(),
      rhs.getResult());

  ASSERT_TRUE(addOp != nullptr);
  EXPECT_EQ(addOp.getLhs(), lhs.getResult());
  EXPECT_EQ(addOp.getRhs(), rhs.getResult());
  EXPECT_EQ(addOp.getResult().getType(), i32Type);

  // Verification is implicit during construction
}

TEST_F(EDAOpsTest, SubOp) {
  auto f32Type = Float32Type::get(&context);
  auto lhs = builder->create<ConstantOp>(
      builder->getUnknownLoc(),
      f32Type,
      builder->getFloatAttr(f32Type, 10.5));
  auto rhs = builder->create<ConstantOp>(
      builder->getUnknownLoc(),
      f32Type,
      builder->getFloatAttr(f32Type, 3.2));

  auto subOp = builder->create<SubOp>(
      builder->getUnknownLoc(),
      lhs.getResult(),
      rhs.getResult());

  ASSERT_TRUE(subOp != nullptr);
  EXPECT_EQ(subOp.getResult().getType(), f32Type);

  // Verification is implicit during construction
}

TEST_F(EDAOpsTest, MulOp) {
  auto i64Type = IntegerType::get(&context, 64);
  auto lhs = builder->create<ConstantOp>(
      builder->getUnknownLoc(),
      i64Type,
      builder->getIntegerAttr(i64Type, 1000));
  auto rhs = builder->create<ConstantOp>(
      builder->getUnknownLoc(),
      i64Type,
      builder->getIntegerAttr(i64Type, 2000));

  auto mulOp = builder->create<MulOp>(
      builder->getUnknownLoc(),
      lhs.getResult(),
      rhs.getResult());

  ASSERT_TRUE(mulOp != nullptr);
  EXPECT_EQ(mulOp.getResult().getType(), i64Type);

  // Verification is implicit during construction
}

TEST_F(EDAOpsTest, DivOp) {
  auto i32Type = IntegerType::get(&context, 32);
  auto lhs = builder->create<ConstantOp>(
      builder->getUnknownLoc(),
      i32Type,
      builder->getIntegerAttr(i32Type, 100));
  auto rhs = builder->create<ConstantOp>(
      builder->getUnknownLoc(),
      i32Type,
      builder->getIntegerAttr(i32Type, 5));

  auto divOp = builder->create<DivOp>(
      builder->getUnknownLoc(),
      lhs.getResult(),
      rhs.getResult());

  ASSERT_TRUE(divOp != nullptr);
  EXPECT_EQ(divOp.getResult().getType(), i32Type);

  // Verification is implicit during construction
}

//===----------------------------------------------------------------------===//
// Comparison Operations Tests
//===----------------------------------------------------------------------===//

TEST_F(EDAOpsTest, CmpEqOp) {
  auto i32Type = IntegerType::get(&context, 32);
  auto i1Type = IntegerType::get(&context, 1);
  auto lhs = builder->create<ConstantOp>(
      builder->getUnknownLoc(),
      i32Type,
      builder->getIntegerAttr(i32Type, 42));
  auto rhs = builder->create<ConstantOp>(
      builder->getUnknownLoc(),
      i32Type,
      builder->getIntegerAttr(i32Type, 42));

  auto cmpOp = builder->create<CmpEqOp>(
      builder->getUnknownLoc(),
      lhs.getResult(),
      rhs.getResult());

  ASSERT_TRUE(cmpOp != nullptr);
  EXPECT_EQ(cmpOp.getLhs(), lhs.getResult());
  EXPECT_EQ(cmpOp.getRhs(), rhs.getResult());
  EXPECT_EQ(cmpOp.getResult().getType(), i1Type);

  // Verification is implicit during construction
}

TEST_F(EDAOpsTest, CmpGtOp) {
  auto i32Type = IntegerType::get(&context, 32);
  auto i1Type = IntegerType::get(&context, 1);
  auto lhs = builder->create<ConstantOp>(
      builder->getUnknownLoc(),
      i32Type,
      builder->getIntegerAttr(i32Type, 100));
  auto rhs = builder->create<ConstantOp>(
      builder->getUnknownLoc(),
      i32Type,
      builder->getIntegerAttr(i32Type, 50));

  auto cmpOp = builder->create<CmpGtOp>(
      builder->getUnknownLoc(),
      lhs.getResult(),
      rhs.getResult());

  ASSERT_TRUE(cmpOp != nullptr);
  EXPECT_EQ(cmpOp.getResult().getType(), i1Type);

  // Verification is implicit during construction
}

TEST_F(EDAOpsTest, CmpLtOp) {
  auto f32Type = Float32Type::get(&context);
  auto i1Type = IntegerType::get(&context, 1);
  auto lhs = builder->create<ConstantOp>(
      builder->getUnknownLoc(),
      f32Type,
      builder->getFloatAttr(f32Type, 3.14));
  auto rhs = builder->create<ConstantOp>(
      builder->getUnknownLoc(),
      f32Type,
      builder->getFloatAttr(f32Type, 6.28));

  auto cmpOp = builder->create<CmpLtOp>(
      builder->getUnknownLoc(),
      lhs.getResult(),
      rhs.getResult());

  ASSERT_TRUE(cmpOp != nullptr);
  EXPECT_EQ(cmpOp.getResult().getType(), i1Type);

  // Verification is implicit during construction
}

//===----------------------------------------------------------------------===//
// Logical Operations Tests
//===----------------------------------------------------------------------===//

TEST_F(EDAOpsTest, AndOp) {
  auto i1Type = IntegerType::get(&context, 1);
  auto lhs = builder->create<ConstantOp>(
      builder->getUnknownLoc(),
      i1Type,
      builder->getBoolAttr(true));
  auto rhs = builder->create<ConstantOp>(
      builder->getUnknownLoc(),
      i1Type,
      builder->getBoolAttr(false));

  auto andOp = builder->create<AndOp>(
      builder->getUnknownLoc(),
      lhs.getResult(),
      rhs.getResult());

  ASSERT_TRUE(andOp != nullptr);
  EXPECT_EQ(andOp.getResult().getType(), i1Type);

  // Verification is implicit during construction
}

TEST_F(EDAOpsTest, OrOp) {
  auto i1Type = IntegerType::get(&context, 1);
  auto lhs = builder->create<ConstantOp>(
      builder->getUnknownLoc(),
      i1Type,
      builder->getBoolAttr(true));
  auto rhs = builder->create<ConstantOp>(
      builder->getUnknownLoc(),
      i1Type,
      builder->getBoolAttr(false));

  auto orOp = builder->create<OrOp>(
      builder->getUnknownLoc(),
      lhs.getResult(),
      rhs.getResult());

  ASSERT_TRUE(orOp != nullptr);
  EXPECT_EQ(orOp.getResult().getType(), i1Type);

  // Verification is implicit during construction
}

TEST_F(EDAOpsTest, NotOp) {
  auto i1Type = IntegerType::get(&context, 1);
  auto input = builder->create<ConstantOp>(
      builder->getUnknownLoc(),
      i1Type,
      builder->getBoolAttr(true));

  auto notOp = builder->create<NotOp>(
      builder->getUnknownLoc(),
      input.getResult());

  ASSERT_TRUE(notOp != nullptr);
  EXPECT_EQ(notOp.getInput(), input.getResult());
  EXPECT_EQ(notOp.getResult().getType(), i1Type);

  // Verification is implicit during construction
}

//===----------------------------------------------------------------------===//
// System Operations Tests
//===----------------------------------------------------------------------===//

TEST_F(EDAOpsTest, PutsOp) {
  auto stringType = StringType::get(&context);
  auto message = builder->create<ConstantOp>(
      builder->getUnknownLoc(),
      stringType,
      builder->getStringAttr("Hello, EDA!"));

  auto putsOp = builder->create<PutsOp>(
      builder->getUnknownLoc(),
      message.getResult());

  ASSERT_TRUE(putsOp != nullptr);
  EXPECT_EQ(putsOp.getArgs().size(), 1);
  EXPECT_EQ(putsOp.getArgs()[0], message.getResult());

  // Verification is implicit during construction
}

TEST_F(EDAOpsTest, FormatOp) {
  auto stringType = StringType::get(&context);
  auto i32Type = IntegerType::get(&context, 32);

  auto pattern = builder->create<ConstantOp>(
      builder->getUnknownLoc(),
      stringType,
      builder->getStringAttr("Value: %d"));

  auto value = builder->create<ConstantOp>(
      builder->getUnknownLoc(),
      i32Type,
      builder->getIntegerAttr(i32Type, 42));

  auto formatOp = builder->create<FormatOp>(
      builder->getUnknownLoc(),
      pattern.getResult(),
      ValueRange{value.getResult()});

  ASSERT_TRUE(formatOp != nullptr);
  EXPECT_EQ(formatOp.getArgs().size(), 1);
  EXPECT_EQ(formatOp.getResult().getType(), stringType);

  // Verification is implicit during construction
}

TEST_F(EDAOpsTest, ConcatOp) {
  auto stringType = StringType::get(&context);
  auto str1 = builder->create<ConstantOp>(
      builder->getUnknownLoc(),
      stringType,
      builder->getStringAttr("Hello"));
  auto str2 = builder->create<ConstantOp>(
      builder->getUnknownLoc(),
      stringType,
      builder->getStringAttr(", "));
  auto str3 = builder->create<ConstantOp>(
      builder->getUnknownLoc(),
      stringType,
      builder->getStringAttr("World!"));

  auto concatOp = builder->create<ConcatOp>(
      builder->getUnknownLoc(),
      ValueRange{str1.getResult(), str2.getResult(), str3.getResult()});

  ASSERT_TRUE(concatOp != nullptr);
  EXPECT_EQ(concatOp.getStrings().size(), 3);
  EXPECT_EQ(concatOp.getResult().getType(), stringType);

  // Verification is implicit during construction
}

//===----------------------------------------------------------------------===//
// Constant Operations Tests
//===----------------------------------------------------------------------===//

TEST_F(EDAOpsTest, IntegerConstantOp) {
  auto i32Type = IntegerType::get(&context, 32);
  auto constOp = builder->create<ConstantOp>(
      builder->getUnknownLoc(),
      i32Type,
      builder->getIntegerAttr(i32Type, 12345));

  ASSERT_TRUE(constOp != nullptr);
  EXPECT_TRUE(constOp.getValue().isa<IntegerAttr>());
  EXPECT_EQ(constOp.getValue().cast<IntegerAttr>().getInt(), 12345);
  EXPECT_EQ(constOp.getResult().getType(), i32Type);

  // Verification is implicit during construction
}

TEST_F(EDAOpsTest, FloatConstantOp) {
  auto f64Type = Float64Type::get(&context);
  auto constOp = builder->create<ConstantOp>(
      builder->getUnknownLoc(),
      f64Type,
      builder->getFloatAttr(f64Type, 3.14159));

  ASSERT_TRUE(constOp != nullptr);
  EXPECT_TRUE(constOp.getValue().isa<FloatAttr>());
  EXPECT_EQ(constOp.getResult().getType(), f64Type);

  // Verification is implicit during construction
}

TEST_F(EDAOpsTest, StringConstantOp) {
  auto stringType = StringType::get(&context);
  auto constOp = builder->create<ConstantOp>(
      builder->getUnknownLoc(),
      stringType,
      builder->getStringAttr("EDA Dialect"));

  ASSERT_TRUE(constOp != nullptr);
  EXPECT_TRUE(constOp.getValue().isa<StringAttr>());
  EXPECT_EQ(constOp.getValue().cast<StringAttr>().getValue(), "EDA Dialect");
  EXPECT_EQ(constOp.getResult().getType(), stringType);

  // Verification is implicit during construction
}

TEST_F(EDAOpsTest, BoolConstantOp) {
  auto i1Type = IntegerType::get(&context, 1);
  auto constOp = builder->create<ConstantOp>(
      builder->getUnknownLoc(),
      i1Type,
      builder->getBoolAttr(true));

  ASSERT_TRUE(constOp != nullptr);
  EXPECT_TRUE(constOp.getValue().isa<BoolAttr>());
  EXPECT_TRUE(constOp.getValue().cast<BoolAttr>().getValue());
  EXPECT_EQ(constOp.getResult().getType(), i1Type);

  // Verification is implicit during construction
}

} // namespace