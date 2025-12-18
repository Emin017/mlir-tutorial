//===-- TclCodeGen.cpp - TCL Code Generation Implementation ---------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "eda-dialect/Transforms/TclCodeGen.h"
#include "eda-dialect/Ops.h"
#include "eda-dialect/Types.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/IR/Operation.h"
#include "mlir/IR/Value.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/Support/raw_ostream.h"

using namespace mlir;
using namespace eda;
using namespace llvm;

namespace {

class TclCodeGenerator {
public:
  TclCodeGenerator(llvm::raw_ostream &os) : os(os) {}

  mlir::LogicalResult translateModule(mlir::ModuleOp module);
  mlir::LogicalResult translateOp(mlir::Operation *op, int indent = 0);

private:
  llvm::raw_ostream &os;
  llvm::StringMap<std::string> valueNames;
  int valueCounter = 0;

  std::string getOrCreateValueName(Value value);
  std::string indent(int level) { return std::string(level * 2, ' '); }
  std::string getTypeName(Type type);
};

std::string TclCodeGenerator::getTypeName(Type type) {
  if (type.isInteger(1)) return "i1";
  if (type.isInteger(8)) return "i8";
  if (type.isInteger(16)) return "i16";
  if (type.isInteger(32)) return "i32";
  if (type.isInteger(64)) return "i64";
  if (type.isF32()) return "f32";
  if (type.isF64()) return "f64";
  if (auto strType = dyn_cast<StringType>(type)) return "string";
  return "any";
}

std::string TclCodeGenerator::getOrCreateValueName(Value value) {
  Operation *defOp = value.getDefiningOp();
  if (!defOp) return "$arg";

  // Create a unique name for this value
  std::string name = "val_" + std::to_string(valueCounter++);
  valueNames[name] = name;
  return name;
}

LogicalResult TclCodeGenerator::translateModule(ModuleOp module) {
  // Iterate through all operations in the module
  for (Operation &op : module.getBody()->getOperations()) {
    if (failed(translateOp(&op, 0))) {
      return failure();
    }
  }
  return success();
}

LogicalResult TclCodeGenerator::translateOp(Operation *op, int indent) {
  // Handle different operation types

  // Handle constant operations
  if (auto constOp = dyn_cast<ConstantOp>(op)) {
    if (auto intAttr = constOp.getValue().dyn_cast<IntegerAttr>()) {
      // For i1 types (booleans), normalize the output
      if (constOp.getResult().getType().isInteger(1)) {
        os << this->indent(indent) << "# constant bool: "
           << (intAttr.getValue() != 0 ? "true" : "false") << "\n";
      } else {
        os << this->indent(indent) << "# constant int: " << intAttr.getInt() << "\n";
      }
    } else if (auto stringAttr = constOp.getValue().dyn_cast<StringAttr>()) {
      os << this->indent(indent) << "set const \"" << stringAttr.getValue() << "\"\n";
    }
    return success();
  }

  if (auto assignOp = dyn_cast<AssignOp>(op)) {
    os << this->indent(indent) << "set " << assignOp.getNameAttr().getValue()
       << " ";

    // Try to inline the value if it's a constant
    Value val = assignOp.getValue();
    if (auto constOp = val.getDefiningOp<ConstantOp>()) {
      if (auto intAttr = constOp.getValue().dyn_cast<IntegerAttr>()) {
        os << intAttr.getInt();
      } else if (auto stringAttr = constOp.getValue().dyn_cast<StringAttr>()) {
        os << "\"" << stringAttr.getValue() << "\"";
      }
    } else {
      // Reference to another variable
      os << "$var";
    }
    os << "\n";
    return success();
  }

  if (auto loadOp = dyn_cast<LoadOp>(op)) {
    os << this->indent(indent) << "set loaded_" << loadOp.getNameAttr().getValue()
       << " $" << loadOp.getNameAttr().getValue() << "\n";
    return success();
  }

  if (auto storeOp = dyn_cast<StoreOp>(op)) {
    os << this->indent(indent) << "set " << storeOp.getNameAttr().getValue()
       << " ";

    // Try to inline the value if it's a constant
    Value val = storeOp.getValue();
    if (auto constOp = val.getDefiningOp<ConstantOp>()) {
      if (auto intAttr = constOp.getValue().dyn_cast<IntegerAttr>()) {
        os << intAttr.getInt();
      } else if (auto stringAttr = constOp.getValue().dyn_cast<StringAttr>()) {
        os << "\"" << stringAttr.getValue() << "\"";
      }
    } else {
      os << "$var";
    }
    os << "\n";
    return success();
  }

  if (auto putsOp = dyn_cast<PutsOp>(op)) {
    os << this->indent(indent) << "puts ";

    // Handle puts arguments
    if (!putsOp.getArgs().empty()) {
      Value arg = putsOp.getArgs()[0];
      if (auto constOp = arg.getDefiningOp<ConstantOp>()) {
        if (auto stringAttr = constOp.getValue().dyn_cast<StringAttr>()) {
          os << "\"" << stringAttr.getValue() << "\"";
        } else if (auto intAttr = constOp.getValue().dyn_cast<IntegerAttr>()) {
          os << intAttr.getInt();
        }
      } else {
        os << "$var";
      }
    }
    os << "\n";
    return success();
  }

  // Handle arithmetic operations
  if (auto addOp = dyn_cast<AddOp>(op)) {
    os << this->indent(indent) << "set result [expr {";

    // Left operand
    Value lhs = addOp.getLhs();
    if (auto constOp = lhs.getDefiningOp<ConstantOp>()) {
      if (auto intAttr = constOp.getValue().dyn_cast<IntegerAttr>()) {
        os << intAttr.getInt();
      }
    } else {
      os << "$lhs";
    }

    os << " + ";

    // Right operand
    Value rhs = addOp.getRhs();
    if (auto constOp = rhs.getDefiningOp<ConstantOp>()) {
      if (auto intAttr = constOp.getValue().dyn_cast<IntegerAttr>()) {
        os << intAttr.getInt();
      }
    } else {
      os << "$rhs";
    }

    os << "}]\n";
    return success();
  }

  // Handle subtraction
  if (auto subOp = dyn_cast<SubOp>(op)) {
    os << this->indent(indent) << "set result [expr {";

    Value lhs = subOp.getLhs();
    if (auto constOp = lhs.getDefiningOp<ConstantOp>()) {
      if (auto intAttr = constOp.getValue().dyn_cast<IntegerAttr>()) {
        os << intAttr.getInt();
      }
    } else {
      os << "$lhs";
    }

    os << " - ";

    Value rhs = subOp.getRhs();
    if (auto constOp = rhs.getDefiningOp<ConstantOp>()) {
      if (auto intAttr = constOp.getValue().dyn_cast<IntegerAttr>()) {
        os << intAttr.getInt();
      }
    } else {
      os << "$rhs";
    }

    os << "}]\n";
    return success();
  }

  // Handle multiplication
  if (auto mulOp = dyn_cast<MulOp>(op)) {
    os << this->indent(indent) << "set result [expr {";

    Value lhs = mulOp.getLhs();
    if (auto constOp = lhs.getDefiningOp<ConstantOp>()) {
      if (auto intAttr = constOp.getValue().dyn_cast<IntegerAttr>()) {
        os << intAttr.getInt();
      }
    } else {
      os << "$lhs";
    }

    os << " * ";

    Value rhs = mulOp.getRhs();
    if (auto constOp = rhs.getDefiningOp<ConstantOp>()) {
      if (auto intAttr = constOp.getValue().dyn_cast<IntegerAttr>()) {
        os << intAttr.getInt();
      }
    } else {
      os << "$rhs";
    }

    os << "}]\n";
    return success();
  }

  // Handle division
  if (auto divOp = dyn_cast<DivOp>(op)) {
    os << this->indent(indent) << "set result [expr {";

    Value lhs = divOp.getLhs();
    if (auto constOp = lhs.getDefiningOp<ConstantOp>()) {
      if (auto intAttr = constOp.getValue().dyn_cast<IntegerAttr>()) {
        os << intAttr.getInt();
      }
    } else {
      os << "$lhs";
    }

    os << " / ";

    Value rhs = divOp.getRhs();
    if (auto constOp = rhs.getDefiningOp<ConstantOp>()) {
      if (auto intAttr = constOp.getValue().dyn_cast<IntegerAttr>()) {
        os << intAttr.getInt();
      }
    } else {
      os << "$rhs";
    }

    os << "}]\n";
    return success();
  }

  // Handle comparison operations
  if (auto cmpEqOp = dyn_cast<CmpEqOp>(op)) {
    os << this->indent(indent) << "set result [expr {";

    Value lhs = cmpEqOp.getLhs();
    if (auto constOp = lhs.getDefiningOp<ConstantOp>()) {
      if (auto intAttr = constOp.getValue().dyn_cast<IntegerAttr>()) {
        os << intAttr.getInt();
      }
    } else {
      os << "$lhs";
    }

    os << " == ";

    Value rhs = cmpEqOp.getRhs();
    if (auto constOp = rhs.getDefiningOp<ConstantOp>()) {
      if (auto intAttr = constOp.getValue().dyn_cast<IntegerAttr>()) {
        os << intAttr.getInt();
      }
    } else {
      os << "$rhs";
    }

    os << "}]\n";
    return success();
  }

  if (auto cmpGtOp = dyn_cast<CmpGtOp>(op)) {
    os << this->indent(indent) << "set result [expr {";

    Value lhs = cmpGtOp.getLhs();
    if (auto constOp = lhs.getDefiningOp<ConstantOp>()) {
      if (auto intAttr = constOp.getValue().dyn_cast<IntegerAttr>()) {
        os << intAttr.getInt();
      }
    } else {
      os << "$lhs";
    }

    os << " > ";

    Value rhs = cmpGtOp.getRhs();
    if (auto constOp = rhs.getDefiningOp<ConstantOp>()) {
      if (auto intAttr = constOp.getValue().dyn_cast<IntegerAttr>()) {
        os << intAttr.getInt();
      }
    } else {
      os << "$rhs";
    }

    os << "}]\n";
    return success();
  }

  if (auto cmpLtOp = dyn_cast<CmpLtOp>(op)) {
    os << this->indent(indent) << "set result [expr {";

    Value lhs = cmpLtOp.getLhs();
    if (auto constOp = lhs.getDefiningOp<ConstantOp>()) {
      if (auto intAttr = constOp.getValue().dyn_cast<IntegerAttr>()) {
        os << intAttr.getInt();
      }
    } else {
      os << "$lhs";
    }

    os << " < ";

    Value rhs = cmpLtOp.getRhs();
    if (auto constOp = rhs.getDefiningOp<ConstantOp>()) {
      if (auto intAttr = constOp.getValue().dyn_cast<IntegerAttr>()) {
        os << intAttr.getInt();
      }
    } else {
      os << "$rhs";
    }

    os << "}]\n";
    return success();
  }

  if (auto ifOp = dyn_cast<IfOp>(op)) {
    os << this->indent(indent) << "if {$condition} {\n";

    // Translate then region
    for (Operation &thenOp : ifOp.getThenRegion().getOps()) {
      if (failed(translateOp(&thenOp, indent + 1))) return failure();
    }

    if (!ifOp.getElseRegion().empty()) {
      os << this->indent(indent) << "} else {\n";
      for (Operation &elseOp : ifOp.getElseRegion().getOps()) {
        if (failed(translateOp(&elseOp, indent + 1))) return failure();
      }
    }

    os << this->indent(indent) << "}\n";
    return success();
  }

  if (auto forOp = dyn_cast<ForOp>(op)) {
    os << this->indent(indent)
       << "for {set i $start} {$i < $end} {incr i} {\n";

    // Translate loop body
    for (Operation &bodyOp : forOp.getBody().getOps()) {
      if (failed(translateOp(&bodyOp, indent + 1))) return failure();
    }

    os << this->indent(indent) << "}\n";
    return success();
  }

  if (auto whileOp = dyn_cast<WhileOp>(op)) {
    os << this->indent(indent) << "while {$condition} {\n";

    // Translate loop body
    for (Operation &bodyOp : whileOp.getBody().getOps()) {
      if (failed(translateOp(&bodyOp, indent + 1))) return failure();
    }

    os << this->indent(indent) << "}\n";
    return success();
  }

  if (auto funcOp = dyn_cast<FuncOp>(op)) {
    os << this->indent(indent) << "proc " << funcOp.getSymName() << " {";
    // TODO: handle function arguments
    os << "} {\n";

    // Translate function body
    for (Operation &bodyOp : funcOp.getBody().getOps()) {
      if (failed(translateOp(&bodyOp, indent + 1))) return failure();
    }

    os << this->indent(indent) << "}\n\n";
    return success();
  }

  if (auto returnOp = dyn_cast<ReturnOp>(op)) {
    os << this->indent(indent) << "return";
    if (!returnOp.getOperands().empty()) {
      os << " $result";
    }
    os << "\n";
    return success();
  }

  // Default: print operation name as comment
  os << this->indent(indent) << "# " << op->getName().getStringRef() << "\n";
  return success();
}

} // namespace

mlir::LogicalResult eda::translateToTcl(ModuleOp module, llvm::raw_ostream &os) {
  TclCodeGenerator generator(os);
  return generator.translateModule(module);
}

mlir::LogicalResult eda::translateOpToTcl(Operation *op, llvm::raw_ostream &os,
                                          int indent) {
  TclCodeGenerator generator(os);
  return generator.translateOp(op, indent);
}
