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

// Parse type with !eda prefix
Type EDADialect::parseType(DialectAsmParser &parser) const {
  llvm::StringRef keyword;
  if (parser.parseKeyword(&keyword))
    return Type();

  if (keyword == "tcl_string")
    return StringType::get(getContext());
  if (keyword == "tcl_list") {
    Type elementType;
    if (parser.parseLess() || parser.parseType(elementType) || parser.parseGreater())
      return Type();
    return ListType::get(getContext(), elementType);
  }
  if (keyword == "tcl_dict") {
    Type keyType, valueType;
    if (parser.parseLess() || parser.parseType(keyType) || parser.parseComma() ||
        parser.parseType(valueType) || parser.parseGreater())
      return Type();
    return DictType::get(getContext(), keyType, valueType);
  }
  if (keyword == "tcl_variable") {
    Type elementType;
    if (parser.parseLess() || parser.parseType(elementType) || parser.parseGreater())
      return Type();
    return VariableType::get(getContext(), elementType);
  }
  if (keyword == "tcl_any")
    return AnyType::get(getContext());

  parser.emitError(parser.getNameLoc(), "unknown type in eda dialect");
  return Type();
}

// Print type with !eda prefix
void EDADialect::printType(Type type, DialectAsmPrinter &printer) const {
  if (type.isa<StringType>()) {
    printer << "tcl_string";
  } else if (auto listType = type.dyn_cast<ListType>()) {
    printer << "tcl_list<";
    printer.printType(listType.getElementType());
    printer << ">";
  } else if (auto dictType = type.dyn_cast<DictType>()) {
    printer << "tcl_dict<";
    printer.printType(dictType.getKeyType());
    printer << ",";
    printer.printType(dictType.getValueType());
    printer << ">";
  } else if (auto varType = type.dyn_cast<VariableType>()) {
    printer << "tcl_variable<";
    printer.printType(varType.getContainedType());
    printer << ">";
  } else if (type.isa<AnyType>()) {
    printer << "tcl_any";
  }
}

// Parse attributes (currently not used, but required by interface)
Attribute EDADialect::parseAttribute(DialectAsmParser &parser, Type type) const {
  return Attribute();
}

// Print attributes (currently not used, but required by interface)
void EDADialect::printAttribute(Attribute attr, DialectAsmPrinter &os) const {
  // Empty - no custom attributes
}

} // namespace eda
