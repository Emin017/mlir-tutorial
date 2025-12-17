//===-- Passes.h - EDA Dialect Pass Declarations --------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef EDA_PASSES_H
#define EDA_PASSES_H

#include "mlir/Pass/Pass.h"

namespace eda {

/// Create a pass for dead code elimination in EDA dialect
std::unique_ptr<mlir::Pass> createDeadCodeEliminationPass();

/// Create a pass for constant folding in EDA dialect
std::unique_ptr<mlir::Pass> createConstantFoldPass();

/// Register all EDA dialect passes
void registerPasses();

} // namespace eda

#endif // EDA_PASSES_H