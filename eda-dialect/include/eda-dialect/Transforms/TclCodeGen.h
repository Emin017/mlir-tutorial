//===-- TclCodeGen.h - TCL Code Generation ---------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file declares the TCL code generation interface.
//
//===----------------------------------------------------------------------===//

#ifndef EDA_TRANSFORMS_TCLCODEGEN_H
#define EDA_TRANSFORMS_TCLCODEGEN_H

#include "mlir/IR/BuiltinOps.h"
#include "llvm/Support/raw_ostream.h"

namespace mlir {
class Operation;
class ModuleOp;
} // namespace mlir

namespace eda {

/// Translate a MLIR module to TCL code
mlir::LogicalResult translateToTcl(mlir::ModuleOp module, llvm::raw_ostream &os);

/// Translate a single operation to TCL code
mlir::LogicalResult translateOpToTcl(mlir::Operation *op, llvm::raw_ostream &os, int indent = 0);

} // namespace eda

#endif // EDA_TRANSFORMS_TCLCODEGEN_H
