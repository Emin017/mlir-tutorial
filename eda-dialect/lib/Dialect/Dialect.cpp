//===-- Dialect.cpp - EDA Dialect Implementation -------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "eda-dialect/Dialect.h"
#include "eda-dialect/Ops.h"
#include "eda-dialect/Types.h"
#include "mlir/IR/Builders.h"
#include "mlir/IR/DialectImplementation.h"
#include "llvm/ADT/TypeSwitch.h"

using namespace mlir;

// Include full type definitions for registration
#define GET_TYPEDEF_CLASSES
#include "eda-dialect/EDATypes.cpp.inc"

#define GET_DIALECT_DEFS
#include "eda-dialect/EDADialect.cpp.inc"

namespace eda {

void EDADialect::initialize() {
  addOperations<
#define GET_OP_LIST
#include "eda-dialect/EDAOps.cpp.inc"
      >();
  addTypes<
#define GET_TYPEDEF_LIST
#include "eda-dialect/EDATypes.cpp.inc"
      >();
}

} // namespace eda
