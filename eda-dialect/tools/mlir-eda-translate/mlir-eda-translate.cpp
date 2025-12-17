//===-- mlir-eda-translate.cpp - EDA MLIR Translator ----------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "eda-dialect/Dialect.h"
#include "eda-dialect/Transforms/TclCodeGen.h"
#include "mlir/Dialect/Arith/IR/Arith.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/IR/DialectRegistry.h"
#include "mlir/IR/MLIRContext.h"
#include "mlir/Support/LogicalResult.h"
#include "mlir/Tools/mlir-translate/MlirTranslateMain.h"
#include "mlir/Tools/mlir-translate/Translation.h"
#include "llvm/Support/InitLLVM.h"

using namespace mlir;

// Translation function
static LogicalResult translateEDAToTcl(ModuleOp module, llvm::raw_ostream &output) {
  return eda::translateToTcl(module, output);
}

// Register the translation
static TranslateFromMLIRRegistration registerEDAToTcl(
    "eda-to-tcl",
    "Translate EDA MLIR dialect to TCL code",
    translateEDAToTcl,
    [](DialectRegistry &registry) {
      registry.insert<eda::EDADialect>();
      registry.insert<func::FuncDialect>();
      registry.insert<arith::ArithDialect>();
    });

int main(int argc, char **argv) {
  llvm::InitLLVM y(argc, argv);
  return mlir::failed(mlirTranslateMain(argc, argv, "EDA MLIR Translator"));
}
