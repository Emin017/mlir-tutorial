//===- EDATranslationTest.cpp - EDA Translation unit tests ----------------===//
//
// Part of the MLIR Tutorial Project
//
//===----------------------------------------------------------------------===//

#include "eda-dialect/Dialect.h"
#include "eda-dialect/Ops.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/IR/Builders.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/IR/BuiltinTypes.h"
#include "mlir/IR/OwningOpRef.h"
#include "mlir/IR/Verifier.h"
#include "gtest/gtest.h"

using namespace mlir;
using namespace eda;

namespace {
class EDATranslationTest : public testing::Test {
protected:
  void SetUp() override {
    context.getOrLoadDialect<EDADialect>();
    context.getOrLoadDialect<func::FuncDialect>();
  }

  // Helper to create a simple module
  OwningOpRef<ModuleOp> createTestModule() {
    OwningOpRef<ModuleOp> module(ModuleOp::create(UnknownLoc::get(&context)));
    OpBuilder builder(&module->getBodyRegion());
    return module;
  }

  // Helper to create an assign operation
  AssignOp createAssignOp(StringRef name, Value value, OpBuilder &builder) {
    return builder.create<AssignOp>(
        builder.getUnknownLoc(),
        name,
        value);
  }

  // Helper to create a constant operation
  ConstantOp createConstant(Type type, Attribute value, OpBuilder &builder) {
    return builder.create<ConstantOp>(
        builder.getUnknownLoc(),
        type,
        value);
  }

  // Helper to translate MLIR to TCL (simplified version)
  std::string translateToTCL(Operation *op) {
    std::string result;
    llvm::raw_string_ostream stream(result);

    // Simple translation for testing
    if (auto assignOp = dyn_cast<AssignOp>(op)) {
      stream << "set " << assignOp.getName() << " ";
      // In a real implementation, we would translate the value
      stream << "<value>";
    } else if (auto constOp = dyn_cast<ConstantOp>(op)) {
      if (auto intAttr = constOp.getValue().dyn_cast<IntegerAttr>()) {
        // For i1 types (booleans), normalize the output
        if (constOp.getResult().getType().isInteger(1)) {
          // Convert any non-zero value to 1, and 0 to 0
          stream << (intAttr.getValue() != 0 ? "1" : "0");
        } else {
          stream << intAttr.getInt();
        }
      } else if (auto stringAttr = constOp.getValue().dyn_cast<StringAttr>()) {
        stream << "\"" << stringAttr.getValue() << "\"";
      }
    } else if (auto putsOp = dyn_cast<PutsOp>(op)) {
      stream << "puts ";
      // In a real implementation, we would translate the arguments
      stream << "<message>";
    }

    return result;
  }

  MLIRContext context;
};

//===----------------------------------------------------------------------===//
// Basic Translation Tests
//===----------------------------------------------------------------------===//

TEST_F(EDATranslationTest, TranslateConstantOperation) {
  auto module = createTestModule();
  OpBuilder builder(&module->getBodyRegion());

  // Create integer constant
  auto i32Type = IntegerType::get(&context, 32);
  auto intConst = createConstant(
      i32Type,
      builder.getIntegerAttr(i32Type, 42),
      builder);

  auto tcl = translateToTCL(intConst);
  EXPECT_EQ(tcl, "42");

  // Create string constant
  auto stringType = StringType::get(&context);
  auto stringConst = createConstant(
      stringType,
      builder.getStringAttr("Hello, World!"),
      builder);

  tcl = translateToTCL(stringConst);
  EXPECT_EQ(tcl, "\"Hello, World!\"");

  // Create boolean constant
  auto i1Type = IntegerType::get(&context, 1);
  auto boolConst = createConstant(
      i1Type,
      builder.getBoolAttr(true),
      builder);

  tcl = translateToTCL(boolConst);
  EXPECT_EQ(tcl, "1");
}

TEST_F(EDATranslationTest, TranslateAssignOperation) {
  auto module = createTestModule();
  OpBuilder builder(&module->getBodyRegion());

  // Create a value to assign
  auto i32Type = IntegerType::get(&context, 32);
  auto value = createConstant(
      i32Type,
      builder.getIntegerAttr(i32Type, 100),
      builder);

  // Create assign operation
  auto assignOp = createAssignOp("timeout", value.getResult(), builder);

  auto tcl = translateToTCL(assignOp);
  EXPECT_EQ(tcl, "set timeout <value>");
}

TEST_F(EDATranslationTest, TranslatePutsOperation) {
  auto module = createTestModule();
  OpBuilder builder(&module->getBodyRegion());

  // Create puts operation
  auto stringType = StringType::get(&context);
  auto message = createConstant(
      stringType,
      builder.getStringAttr("Hello, EDA!"),
      builder);

  auto putsOp = builder.create<PutsOp>(
      builder.getUnknownLoc(),
      message.getResult());

  auto tcl = translateToTCL(putsOp);
  EXPECT_EQ(tcl, "puts <message>");
}

//===----------------------------------------------------------------------===//
// Complex Translation Tests
//===----------------------------------------------------------------------===//

TEST_F(EDATranslationTest, TranslateSimpleModule) {
  auto module = createTestModule();
  OpBuilder builder(&module->getBodyRegion());

  // Create a simple sequence of operations
  auto i32Type = IntegerType::get(&context, 32);
  auto timeoutValue = createConstant(
      i32Type,
      builder.getIntegerAttr(i32Type, 10),
      builder);

  auto assignTimeout = createAssignOp("timeout", timeoutValue.getResult(), builder);

  auto stringType = StringType::get(&context);
  auto message = createConstant(
      stringType,
      builder.getStringAttr("Timeout set"),
      builder);

  auto putsMessage = builder.create<PutsOp>(
      builder.getUnknownLoc(),
      message.getResult());

  // Verify module
  EXPECT_TRUE(succeeded(mlir::verify(*module)));

  // Translate each operation
  std::vector<std::string> translations;
  module->walk([&](Operation *op) {
    if (!isa<ModuleOp>(op)) {
      auto tcl = translateToTCL(op);
      if (!tcl.empty()) {
        translations.push_back(tcl);
      }
    }
  });

  // Verify translations
  EXPECT_EQ(translations.size(), 4);  // 2 constants + 1 assign + 1 puts
  EXPECT_EQ(translations[0], "10");
  EXPECT_EQ(translations[1], "set timeout <value>");
  EXPECT_EQ(translations[2], "\"Timeout set\"");
  EXPECT_EQ(translations[3], "puts <message>");
}

TEST_F(EDATranslationTest, TranslateArithmeticOperations) {
  auto module = createTestModule();
  OpBuilder builder(&module->getBodyRegion());

  auto i32Type = IntegerType::get(&context, 32);

  // Create constants
  auto const1 = createConstant(
      i32Type,
      builder.getIntegerAttr(i32Type, 10),
      builder);
  auto const2 = createConstant(
      i32Type,
      builder.getIntegerAttr(i32Type, 20),
      builder);

  // Create arithmetic operations
  auto addOp = builder.create<AddOp>(
      builder.getUnknownLoc(),
      const1.getResult(),
      const2.getResult());

  auto subOp = builder.create<SubOp>(
      builder.getUnknownLoc(),
      const1.getResult(),
      const2.getResult());

  auto mulOp = builder.create<MulOp>(
      builder.getUnknownLoc(),
      const1.getResult(),
      const2.getResult());

  // Verify module
  EXPECT_TRUE(succeeded(mlir::verify(*module)));

  // Count operations
  int constCount = 0, arithCount = 0;
  module->walk([&](Operation *op) {
    if (isa<ConstantOp>(op))
      constCount++;
    else if (isa<AddOp, SubOp, MulOp, DivOp>(op))
      arithCount++;
  });

  EXPECT_EQ(constCount, 2);
  EXPECT_EQ(arithCount, 3);
}

TEST_F(EDATranslationTest, TranslateComparisonOperations) {
  auto module = createTestModule();
  OpBuilder builder(&module->getBodyRegion());

  auto i32Type = IntegerType::get(&context, 32);
  auto i1Type = IntegerType::get(&context, 1);

  // Create constants
  auto const1 = createConstant(
      i32Type,
      builder.getIntegerAttr(i32Type, 10),
      builder);
  auto const2 = createConstant(
      i32Type,
      builder.getIntegerAttr(i32Type, 20),
      builder);

  // Create comparison operations
  auto cmpEq = builder.create<CmpEqOp>(
      builder.getUnknownLoc(),
      const1.getResult(),
      const2.getResult());

  auto cmpGt = builder.create<CmpGtOp>(
      builder.getUnknownLoc(),
      const1.getResult(),
      const2.getResult());

  auto cmpLt = builder.create<CmpLtOp>(
      builder.getUnknownLoc(),
      const1.getResult(),
      const2.getResult());

  // Verify module
  EXPECT_TRUE(succeeded(mlir::verify(*module)));

  // Check operation types
  EXPECT_TRUE(cmpEq.getResult().getType().isa<IntegerType>());
  EXPECT_TRUE(cmpGt.getResult().getType().isa<IntegerType>());
  EXPECT_TRUE(cmpLt.getResult().getType().isa<IntegerType>());

  // Count operations
  int constCount = 0, cmpCount = 0;
  module->walk([&](Operation *op) {
    if (isa<ConstantOp>(op))
      constCount++;
    else if (isa<CmpEqOp, CmpGtOp, CmpLtOp>(op))
      cmpCount++;
  });

  EXPECT_EQ(constCount, 2);
  EXPECT_EQ(cmpCount, 3);
}

TEST_F(EDATranslationTest, TranslateLogicalOperations) {
  auto module = createTestModule();
  OpBuilder builder(&module->getBodyRegion());

  auto i1Type = IntegerType::get(&context, 1);

  // Create boolean constants
  auto trueConst = createConstant(
      i1Type,
      builder.getBoolAttr(true),
      builder);
  auto falseConst = createConstant(
      i1Type,
      builder.getBoolAttr(false),
      builder);

  // Create logical operations
  auto andOp = builder.create<AndOp>(
      builder.getUnknownLoc(),
      trueConst.getResult(),
      falseConst.getResult());

  auto orOp = builder.create<OrOp>(
      builder.getUnknownLoc(),
      trueConst.getResult(),
      falseConst.getResult());

  auto notOp = builder.create<NotOp>(
      builder.getUnknownLoc(),
      trueConst.getResult());

  // Verify module
  EXPECT_TRUE(succeeded(mlir::verify(*module)));

  // Count operations
  int constCount = 0, logicalCount = 0;
  module->walk([&](Operation *op) {
    if (isa<ConstantOp>(op))
      constCount++;
    else if (isa<AndOp, OrOp, NotOp>(op))
      logicalCount++;
  });

  EXPECT_EQ(constCount, 2);
  EXPECT_EQ(logicalCount, 3);
}

TEST_F(EDATranslationTest, TranslateModuleWithFunction) {
  auto module = createTestModule();
  OpBuilder builder(&module->getBodyRegion());

  // Create a function
  auto funcType = builder.getFunctionType(
      TypeRange{IntegerType::get(&context, 32)},
      TypeRange{IntegerType::get(&context, 32)});
  auto func = func::FuncOp::create(
      builder.getUnknownLoc(),
      "test_function",
      funcType);

  module->push_back(func);

  // Add function body
  auto *entryBlock = func.addEntryBlock();
  builder.setInsertionPointToEnd(entryBlock);

  // Simple operation in function
  auto retVal = builder.create<AddOp>(
      builder.getUnknownLoc(),
      entryBlock->getArgument(0),
      entryBlock->getArgument(0));

  builder.create<func::ReturnOp>(
      builder.getUnknownLoc(),
      retVal.getResult());

  // Verify module
  EXPECT_TRUE(succeeded(mlir::verify(*module)));

  // Check function exists
  auto foundFunc = module->lookupSymbol<func::FuncOp>("test_function");
  EXPECT_TRUE(foundFunc != nullptr);
  EXPECT_EQ(foundFunc.getName(), "test_function");
}

} // namespace