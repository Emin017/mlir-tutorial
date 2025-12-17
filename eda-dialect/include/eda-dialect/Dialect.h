//===-- Dialect.h - EDA Dialect Definition -------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef EDA_DIALECT_H
#define EDA_DIALECT_H

#include "mlir/IR/Dialect.h"
#include "mlir/IR/BuiltinTypes.h"
#include "mlir/IR/OpDefinition.h"
#include "mlir/IR/OpImplementation.h"

// Forward declarations
namespace eda {
class EDADialect;
} // namespace eda

// Include the generated dialect base definition
#include "eda-dialect/EDADialect.h.inc"

// Include types and operations
#include "eda-dialect/Types.h"
#include "eda-dialect/Ops.h"

#endif // EDA_DIALECT_H