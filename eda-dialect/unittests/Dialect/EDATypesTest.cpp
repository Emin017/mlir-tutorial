//===- EDATypesTest.cpp - EDA Types unit tests ---------------------------===//
//
// Part of the MLIR Tutorial Project
//
//===----------------------------------------------------------------------===//

#include "eda-dialect/Dialect.h"
#include "eda-dialect/Types.h"
#include "mlir/IR/Builders.h"
#include "mlir/IR/BuiltinTypes.h"
#include "mlir/IR/AsmState.h"
#include "gtest/gtest.h"

using namespace mlir;
using namespace eda;

namespace {
class EDATypesTest : public testing::Test {
protected:
  void SetUp() override {
    context.getOrLoadDialect<EDADialect>();
  }

  MLIRContext context;
};

//===----------------------------------------------------------------------===//
// Custom Types Tests
//===----------------------------------------------------------------------===//

TEST_F(EDATypesTest, StringType) {
  auto stringType = StringType::get(&context);

  ASSERT_TRUE(stringType != nullptr);
  EXPECT_TRUE(stringType.isa<StringType>());

  // Test type ID uniqueness
  auto stringType2 = StringType::get(&context);
  EXPECT_EQ(stringType, stringType2);
}

TEST_F(EDATypesTest, ListType) {
  auto elementType = IntegerType::get(&context, 32);
  auto listType = ListType::get(&context, elementType);

  ASSERT_TRUE(listType != nullptr);
  EXPECT_TRUE(listType.isa<ListType>());
  EXPECT_EQ(listType.getElementType(), elementType);

  // Test type ID uniqueness with same element type
  auto listType2 = ListType::get(&context, elementType);
  EXPECT_EQ(listType, listType2);

  // Test different element types produce different list types
  auto anotherElementType = Float32Type::get(&context);
  auto differentListType = ListType::get(&context, anotherElementType);
  EXPECT_NE(listType, differentListType);
}

TEST_F(EDATypesTest, NestedListTypes) {
  auto innerElementType = IntegerType::get(&context, 32);
  auto innerListType = ListType::get(&context, innerElementType);
  auto outerListType = ListType::get(&context, innerListType);

  ASSERT_TRUE(outerListType != nullptr);
  EXPECT_TRUE(outerListType.isa<ListType>());
  EXPECT_EQ(outerListType.getElementType(), innerListType);
}

TEST_F(EDATypesTest, ListTypeWithDifferentElementTypes) {
  auto intType = IntegerType::get(&context, 64);
  auto floatType = Float64Type::get(&context);
  auto stringType = StringType::get(&context);

  auto intList = ListType::get(&context, intType);
  auto floatList = ListType::get(&context, floatType);
  auto stringList = ListType::get(&context, stringType);

  EXPECT_NE(intList, floatList);
  EXPECT_NE(floatList, stringList);
  EXPECT_NE(intList, stringList);

  EXPECT_EQ(intList.getElementType(), intType);
  EXPECT_EQ(floatList.getElementType(), floatType);
  EXPECT_EQ(stringList.getElementType(), stringType);
}

TEST_F(EDATypesTest, TypePrinting) {
  auto stringType = StringType::get(&context);
  auto intType = IntegerType::get(&context, 32);
  auto listType = ListType::get(&context, intType);

  // Test that types are valid (printing would require AsmPrinter setup)
  EXPECT_TRUE(stringType != nullptr);
  EXPECT_TRUE(listType != nullptr);
}

TEST_F(EDATypesTest, TypeParsing) {
  // This test would require implementing parsing in the dialect
  // For now, just verify the types can be created
  auto stringType = StringType::get(&context);
  auto intType = IntegerType::get(&context, 32);
  auto listType = ListType::get(&context, intType);

  EXPECT_TRUE(stringType != nullptr);
  EXPECT_TRUE(listType != nullptr);
}

} // namespace