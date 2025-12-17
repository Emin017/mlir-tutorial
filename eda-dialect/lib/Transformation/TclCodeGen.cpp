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
  if (auto assignOp = dyn_cast<AssignOp>(op)) {
    os << this->indent(indent) << "set " << assignOp.getNameAttr().getValue()
       << " ";
    // TODO: handle value
    os << "\n";
    return success();
  }

  if (auto loadOp = dyn_cast<LoadOp>(op)) {
    os << this->indent(indent) << "# Load variable: " << loadOp.getNameAttr().getValue() << "\n";
    return success();
  }

  if (auto storeOp = dyn_cast<StoreOp>(op)) {
    os << this->indent(indent) << "set " << storeOp.getNameAttr().getValue()
       << " $value\n";
    return success();
  }

  if (auto putsOp = dyn_cast<PutsOp>(op)) {
    os << this->indent(indent) << "puts \"";
    // TODO: handle puts arguments
    os << "\"\n";
    return success();
  }

  if (auto addOp = dyn_cast<AddOp>(op)) {
    os << this->indent(indent) << "# add operation\n";
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
