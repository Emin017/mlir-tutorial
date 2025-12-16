//===-- Types.cpp - EDA Type Implementation ------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "eda-dialect/Types.h"
#include "mlir/IR/DialectImplementation.h"
#include "llvm/ADT/TypeSwitch.h"

using namespace mlir;
namespace eda {

//===----------------------------------------------------------------------===//
// StringType
//===----------------------------------------------------------------------===//

bool StringType::classof(Type type) {
  return type.isa<::mlir::IntegerType, ::mlir::FloatType, ::mlir::StringType>();
}

//===----------------------------------------------------------------------===//
// ListType
//===----------------------------------------------------------------------===//

LogicalResult ListType::verify(
    function_ref<InFlightDiagnostic()> emitError,
    Type elementType) {
  if (!elementType)
    return emitError() << "list element type cannot be null";
  return success();
}

//===----------------------------------------------------------------------===//
// DictType
//===----------------------------------------------------------------------===//

LogicalResult DictType::verify(
    function_ref<InFlightDiagnostic()> emitError,
    Type keyType, Type valueType) {
  if (!keyType)
    return emitError() << "dict key type cannot be null";
  if (!valueType)
    return emitError() << "dict value type cannot be null";
  return success();
}

//===----------------------------------------------------------------------===//
// VariableType
//===----------------------------------------------------------------------===//

LogicalResult VariableType::verify(
    function_ref<InFlightDiagnostic()> emitError,
    Type containedType) {
  if (!containedType)
    return emitError() << "variable contained type cannot be null";
  return success();
}

} // namespace eda