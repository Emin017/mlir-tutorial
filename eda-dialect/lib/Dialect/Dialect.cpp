//===-- Dialect.cpp - EDA Dialect Implementation -------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "eda-dialect/Dialect.h"
#include "eda-dialect/Types.h"
#include "mlir/IR/Builders.h"
#include "mlir/IR/DialectImplementation.h"

using namespace mlir;

namespace eda {

EDADialect::EDADialect(MLIRContext *context)
    : Dialect(getDialectNamespace(), context, TypeID::get<EDADialect>()) {
  registerTypes();
}

mlir::Type EDADialect::parseType(DialectAsmParser &parser) const {
  StringRef keyword;
  if (parser.parseKeyword(&keyword))
    return nullptr;

  // Parse the different types
  if (keyword == "tcl_string") {
    return StringType::get(getContext());
  } else if (keyword == "tcl_list") {
    if (parser.parseLess())
      return nullptr;
    Type elementType;
    if (parser.parseType(elementType))
      return nullptr;
    if (parser.parseGreater())
      return nullptr;
    return ListType::get(elementType);
  } else if (keyword == "tcl_dict") {
    if (parser.parseLess())
      return nullptr;
    Type keyType, valueType;
    if (parser.parseType(keyType))
      return nullptr;
    if (parser.parseComma())
      return nullptr;
    if (parser.parseType(valueType))
      return nullptr;
    if (parser.parseGreater())
      return nullptr;
    return DictType::get(keyType, valueType);
  } else if (keyword == "tcl_variable") {
    if (parser.parseLess())
      return nullptr;
    Type containedType;
    if (parser.parseType(containedType))
      return nullptr;
    if (parser.parseGreater())
      return nullptr;
    return VariableType::get(containedType);
  } else if (keyword == "tcl_any") {
    return AnyType::get(getContext());
  }

  parser.emitError(parser.getNameLoc()) << "unknown EDA type: " << keyword;
  return nullptr;
}

void EDADialect::printType(Type type, DialectAsmPrinter &printer) const {
  if (auto stringType = type.dyn_cast<StringType>()) {
    printer << "tcl_string";
  } else if (auto listType = type.dyn_cast<ListType>()) {
    printer << "tcl_list<" << listType.getElementType() << ">";
  } else if (auto dictType = type.dyn_cast<DictType>()) {
    printer << "tcl_dict<" << dictType.getKeyType() << ", "
            << dictType.getValueType() << ">";
  } else if (auto variableType = type.dyn_cast<VariableType>()) {
    printer << "tcl_variable<" << variableType.getContainedType() << ">";
  } else if (type.isa<AnyType>()) {
    printer << "tcl_any";
  } else {
    llvm_unreachable("unexpected 'eda' type kind");
  }
}

void EDADialect::registerTypes() {
  addTypes<StringType, ListType, DictType, VariableType, AnyType>();
}

} // namespace eda