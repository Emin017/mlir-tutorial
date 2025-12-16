//===-- mlir-eda-translate.cpp - EDA Dialect translation tool ------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements a command-line tool that translates MLIR code using
// the EDA Dialect to TCL code.
//
//===----------------------------------------------------------------------===//

#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/IR/DialectRegistry.h"
#include "mlir/IR/MLIRContext.h"
#include "mlir/InitAllDialects.h"
#include "mlir/Pass/PassManager.h"
#include "mlir/Support/FileUtilities.h"
#include "mlir/Tools/mlir-translate/MlirTranslateMain.h"

#include "eda-dialect/Dialect.h"
#include "eda-dialect/Ops.h"

#include "llvm/Support/CommandLine.h"
#include "llvm/Support/InitLLVM.h"
#include "llvm/Support/raw_ostream.h"

using namespace mlir;
using namespace eda;

namespace {

class TCLTranslation {
public:
  explicit TCLTranslation(raw_ostream &os) : os(os) {}

  LogicalResult translateModule(ModuleOp module);

private:
  LogicalResult translateOperation(Operation *op, int indent = 0);
  void printIndent(int indent);
  LogicalResult translateAssignOp(AssignOp op, int indent);
  LogicalResult translateLoadOp(LoadOp op, int indent);
  LogicalResult translateStoreOp(StoreOp op, int indent);
  LogicalResult translateArithmeticOp(Operation *op, StringRef opName, int indent);
  LogicalResult translateComparisonOp(Operation *op, StringRef opName, int indent);
  LogicalResult translatePutsOp(PutsOp op, int indent);
  LogicalResult translateIfOp(IfOp op, int indent);
  LogicalResult translateForOp(ForOp op, int indent);
  LogicalResult translateWhileOp(WhileOp op, int indent);
  LogicalResult translateFuncOp(FuncOp func, int indent);
  LogicalResult translateCallOp(CallOp op, int indent);
  LogicalResult translateReturnOp(ReturnOp op, int indent);
  LogicalResult translateFileOp(Operation *op, StringRef opName, int indent);
  LogicalResult translateConstantOp(ConstantOp op);

  raw_ostream &os;
  llvm::DenseMap<Value, std::string> valueNames;
  int valueCounter = 0;
};

LogicalResult TCLTranslation::translateModule(ModuleOp module) {
  os << "# Generated TCL code from MLIR EDA Dialect\n";
  os << "# by mlir-eda-translate\n\n";

  for (Operation &op : module.getBody()->getOperations()) {
    if (failed(translateOperation(&op)))
      return failure();
  }

  return success();
}

LogicalResult TCLTranslation::translateOperation(Operation *op, int indent) {
  // Skip module terminator
  if (op->hasTrait<OpTrait::IsTerminator>())
    return success();

  // Handle different operation types
  if (auto assignOp = dyn_cast<AssignOp>(op))
    return translateAssignOp(assignOp, indent);
  else if (auto loadOp = dyn_cast<LoadOp>(op))
    return translateLoadOp(loadOp, indent);
  else if (auto storeOp = dyn_cast<StoreOp>(op))
    return translateStoreOp(storeOp, indent);
  else if (auto putsOp = dyn_cast<PutsOp>(op))
    return translatePutsOp(putsOp, indent);
  else if (auto ifOp = dyn_cast<IfOp>(op))
    return translateIfOp(ifOp, indent);
  else if (auto forOp = dyn_cast<ForOp>(op))
    return translateForOp(forOp, indent);
  else if (auto whileOp = dyn_cast<WhileOp>(op))
    return translateWhileOp(whileOp, indent);
  else if (auto funcOp = dyn_cast<FuncOp>(op))
    return translateFuncOp(funcOp, indent);
  else if (auto callOp = dyn_cast<CallOp>(op))
    return translateCallOp(callOp, indent);
  else if (auto returnOp = dyn_cast<ReturnOp>(op))
    return translateReturnOp(returnOp, indent);
  else if (auto constantOp = dyn_cast<ConstantOp>(op))
    return translateConstantOp(constantOp);
  else {
    // Handle generic operations by name
    StringRef opName = op->getName().stripDialect();
    if (op->getName().getDialectNamespace() == "eda") {
      if (opName == "add" || opName == "sub" || opName == "mul" || opName == "div")
        return translateArithmeticOp(op, opName, indent);
      else if (opName.startswith("cmp_"))
        return translateComparisonOp(op, opName, indent);
      else if (opName.startswith("file_"))
        return translateFileOp(op, opName, indent);
    }
  }

  op->dump();
  return op->emitError("unsupported operation for TCL translation");
}

void TCLTranslation::printIndent(int indent) {
  for (int i = 0; i < indent; ++i)
    os << "    ";
}

LogicalResult TCLTranslation::translateAssignOp(AssignOp op, int indent) {
  printIndent(indent);
  StringRef varName = op.getName();

  // Generate a name for the value if needed
  std::string valueName;
  if (auto constOp = op.getValue().getDefiningOp<ConstantOp>()) {
    // Inline constant
    if (auto intAttr = constOp.getValue().dyn_cast<IntegerAttr>()) {
      valueName = std::to_string(intAttr.getInt());
    } else if (auto stringAttr = constOp.getValue().dyn_cast<StringAttr>()) {
      valueName = "\"" + stringAttr.getValue().str() + "\"";
    }
  } else {
    // Reference to variable
    valueName = "$" + varName.str();
  }

  os << "set " << varName << " " << valueName << "\n";
  return success();
}

LogicalResult TCLTranslation::translateLoadOp(LoadOp op, int indent) {
  // In TCL, loading a variable is just referencing it
  // This operation is usually folded into other operations
  return success();
}

LogicalResult TCLTranslation::translateStoreOp(StoreOp op, int indent) {
  printIndent(indent);
  StringRef varName = op.getName();

  // Generate a name for the value
  std::string valueName;
  auto &value = op.getValue();
  if (auto constOp = value.getDefiningOp<ConstantOp>()) {
    if (auto intAttr = constOp.getValue().dyn_cast<IntegerAttr>()) {
      valueName = std::to_string(intAttr.getInt());
    } else if (auto stringAttr = constOp.getValue().dyn_cast<StringAttr>()) {
      valueName = "\"" + stringAttr.getValue().str() + "\"";
    }
  } else {
    // It's a variable reference
    valueName = "$" + varName.str();
  }

  os << "set " << varName << " " << valueName << "\n";
  return success();
}

LogicalResult TCLTranslation::translateArithmeticOp(Operation *op, StringRef opName, int indent) {
  printIndent(indent);

  // Generate variable name for result
  std::string resultVar = "tmp_" + std::to_string(valueCounter++);

  // Map operands to variable names
  std::string lhs, rhs;
  if (auto constOp = op->getOperand(0).getDefiningOp<ConstantOp>()) {
    if (auto intAttr = constOp.getValue().dyn_cast<IntegerAttr>()) {
      lhs = std::to_string(intAttr.getInt());
    }
  } else {
    lhs = "$" + std::string(op->getOperand(0).getType().isInteger(1) ? "bool" : "var");
  }

  if (auto constOp = op->getOperand(1).getDefiningOp<ConstantOp>()) {
    if (auto intAttr = constOp.getValue().dyn_cast<IntegerAttr>()) {
      rhs = std::to_string(intAttr.getInt());
    }
  } else {
    rhs = "$" + std::string(op->getOperand(1).getType().isInteger(1) ? "bool" : "var");
  }

  // Map operation to TCL expr
  StringRef tclOp;
  if (opName == "add") tclOp = "+";
  else if (opName == "sub") tclOp = "-";
  else if (opName == "mul") tclOp = "*";
  else if (opName == "div") tclOp = "/";
  else return op->emitError("unknown arithmetic operation");

  os << "set " << resultVar << " [expr {" << lhs << " " << tclOp << " " << rhs << "}]\n";
  valueNames[op->getResult(0)] = resultVar;
  return success();
}

LogicalResult TCLTranslation::translateComparisonOp(Operation *op, StringRef opName, int indent) {
  printIndent(indent);

  std::string resultVar = "tmp_" + std::to_string(valueCounter++);

  StringRef tclOp;
  if (opName == "cmp_eq") tclOp = "==";
  else if (opName == "cmp_gt") tclOp = ">";
  else if (opName == "cmp_lt") tclOp = "<";
  else return op->emitError("unknown comparison operation");

  os << "set " << resultVar << " [expr {$var1 " << tclOp << " $var2}]\n";
  valueNames[op->getResult(0)] = resultVar;
  return success();
}

LogicalResult TCLTranslation::translatePutsOp(PutsOp op, int indent) {
  printIndent(indent);

  if (op.getArgs().size() == 1) {
    // Simple string or variable
    auto arg = op.getArgs()[0];
    if (auto constOp = arg.getDefiningOp<ConstantOp>()) {
      if (auto stringAttr = constOp.getValue().dyn_cast<StringAttr>()) {
        os << "puts \"" << stringAttr.getValue() << "\"\n";
      }
    } else {
      os << "puts $var\n";
    }
  } else {
    // Formatted output (simplified)
    os << "puts [format \"$args\"]\n";
  }

  return success();
}

LogicalResult TCLTranslation::translateIfOp(IfOp op, int indent) {
  printIndent(indent);
  os << "if {$condition} {\n";

  // Translate then region
  for (Operation &bodyOp : op.getThenRegion().front().getOperations()) {
    if (!bodyOp.hasTrait<OpTrait::IsTerminator>()) {
      if (failed(translateOperation(&bodyOp, indent + 1)))
        return failure();
    }
  }

  if (!op.getElseRegion().empty()) {
    printIndent(indent);
    os << "} else {\n";

    for (Operation &bodyOp : op.getElseRegion().front().getOperations()) {
      if (!bodyOp.hasTrait<OpTrait::IsTerminator>()) {
        if (failed(translateOperation(&bodyOp, indent + 1)))
          return failure();
      }
    }
  }

  printIndent(indent);
  os << "}\n";
  return success();
}

LogicalResult TCLTranslation::translateForOp(ForOp op, int indent) {
  printIndent(indent);
  os << "for {set i " << op.getStart() << "} {$i < " << op.getEnd() << "} {incr i} {\n";

  // Translate body
  for (Operation &bodyOp : op.getBody().front().getOperations()) {
    if (!bodyOp.hasTrait<OpTrait::IsTerminator>()) {
      if (failed(translateOperation(&bodyOp, indent + 1)))
        return failure();
    }
  }

  printIndent(indent);
  os << "}\n";
  return success();
}

LogicalResult TCLTranslation::translateFuncOp(FuncOp func, int indent) {
  printIndent(indent);
  os << "proc " << func.getName() << " {";

  // Print parameters
  auto argTypes = func.getArgumentTypes();
  for (size_t i = 0; i < argTypes.size(); ++i) {
    if (i > 0) os << " ";
    os << "arg" << i;
  }

  os << "} {\n";

  // Translate function body
  for (Operation &op : func.getBody().front().getOperations()) {
    if (!op.hasTrait<OpTrait::IsTerminator>()) {
      if (failed(translateOperation(&op, indent + 1)))
        return failure();
    }
  }

  printIndent(indent);
  os << "}\n\n";
  return success();
}

LogicalResult TCLTranslation::translateCallOp(CallOp op, int indent) {
  printIndent(indent);

  std::string resultVar = "tmp_" + std::to_string(valueCounter++);
  os << "set " << resultVar << " [" << op.getCallee() << " $args]\n";
  valueNames[op.getResult(0)] = resultVar;
  return success();
}

LogicalResult TCLTranslation::translateReturnOp(ReturnOp op, int indent) {
  printIndent(indent);

  if (op.getOperands().empty()) {
    os << "return\n";
  } else if (op.getNumOperands() == 1) {
    auto operand = op.getOperands()[0];
    if (auto constOp = operand.getDefiningOp<ConstantOp>()) {
      if (auto intAttr = constOp.getValue().dyn_cast<IntegerAttr>()) {
        os << "return " << intAttr.getInt() << "\n";
      }
    } else {
      os << "return $var\n";
    }
  } else {
    os << "return [list $args]\n";
  }

  return success();
}

LogicalResult TCLTranslation::translateFileOp(Operation *op, StringRef opName, int indent) {
  printIndent(indent);

  if (opName == "file_exists") {
    os << "set result [file exists $path]\n";
  } else if (opName == "file_read") {
    os << "set result [read $filename]\n";
  } else if (opName == "file_write") {
    os << "write $filename $content\n";
  } else if (opName == "file_glob") {
    os << "set result [glob $pattern]\n";
  }

  return success();
}

LogicalResult TCLTranslation::translateConstantOp(ConstantOp op) {
  // Constants are inlined where used
  return success();
}

LogicalResult translateToTCLOperation(Operation *op, raw_ostream &os) {
  TCLTranslation translator(os);
  if (auto module = dyn_cast<ModuleOp>(op)) {
    return translator.translateModule(module);
  }
  return failure();
}

} // namespace

int main(int argc, char **argv) {
  InitLLVM y(argc, argv);

  mlir::DialectRegistry registry;
  registry.insert<eda::EDADialect>();
  registry.insert<mlir::func::FuncDialect>();

  mlir::registerTranslationToLLVMIR();
  mlir::registerFromLLVMIRTranslation();

  // Register our translation
  mlir::TranslateToMLIRRegistration registration(
      "mlir-to-tcl",
      "translate MLIR EDA Dialect to TCL",
      [](llvm::SourceMgr &sourceMgr, mlir::MLIRContext *context) {
        return mlir::translateFromMLIR(
            sourceMgr, *context, translateToTCLOperation);
      });

  return mlir::mlirTranslateMain(argc, argv, "EDA Dialect to TCL translator");
}