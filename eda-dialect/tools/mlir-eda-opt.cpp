//===-- mlir-eda-opt.cpp - EDA Dialect optimization tool -----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements a command-line tool that optimizes MLIR code using
// the EDA Dialect transformations.
//
//===----------------------------------------------------------------------===//

#include "mlir/IR/AsmState.h"
#include "mlir/IR/DialectRegistry.h"
#include "mlir/IR/MLIRContext.h"
#include "mlir/InitAllDialects.h"
#include "mlir/InitAllPasses.h"
#include "mlir/Pass/Pass.h"
#include "mlir/Pass/PassManager.h"
#include "mlir/Support/FileUtilities.h"
#include "mlir/Tools/mlir-opt/MlirOptMain.h"

#include "eda-dialect/Dialect.h"

using namespace mlir;

int main(int argc, char **argv) {
  registerAllPasses();

  // Register EDA Dialect
  DialectRegistry registry;
  registry.insert<eda::EDADialect>();

  // Register command line options for EDA passes
  PassPipelineRegistration<>("eda-opt",
                             "EDA Dialect optimization pipeline",
                             [](OpPassManager &pm) {
                               // Add optimization passes here
                               pm.addPass(createCanonicalizerPass());
                               pm.addPass(createCSEPass());
                               // Future: add custom EDA passes
                             });

  return mlirOptMain(argc, argv, "EDA Dialect optimizer\n", registry);
}