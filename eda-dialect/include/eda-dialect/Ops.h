//===-- Ops.h - EDA Operation Declarations -------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef EDA_OPS_H
#define EDA_OPS_H

#include "mlir/Bytecode/BytecodeOpInterface.h"
#include "mlir/IR/Attributes.h"
#include "mlir/IR/BuiltinTypes.h"
#include "mlir/IR/Dialect.h"
#include "mlir/IR/OpDefinition.h"
#include "mlir/IR/Region.h"
#include "mlir/IR/Operation.h"
#include "mlir/IR/OperationSupport.h"
#include "mlir/IR/Value.h"
#include "mlir/IR/ValueRange.h"
#include "mlir/IR/TypeRange.h"
#include "mlir/Interfaces/CallInterfaces.h"
#include "mlir/Interfaces/FunctionInterfaces.h"
#include "mlir/Interfaces/ControlFlowInterfaces.h"
#include "mlir/Interfaces/InferTypeOpInterface.h"
#include "mlir/Interfaces/LoopLikeInterface.h"
#include "mlir/Interfaces/SideEffectInterfaces.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/StringRef.h"

// Forward declare types from Types.h
namespace eda {
class StringType;
class ListType;
class DictType;
class VariableType;
class AnyType;
}

// Note: Some types (StringRef, Value, etc.) are used by generated TableGen code
// which may introduce both unqualified and qualified forms. Avoid importing them
// into the global namespace to prevent duplicate declaration conflicts.

// Include generated operation definitions
#define GET_OP_CLASSES
#include "eda-dialect/EDAOps.h.inc"

#endif // EDA_OPS_H