//===-- Ops.cpp - EDA Operation Implementation ---------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "eda-dialect/Ops.h"
#include "eda-dialect/Dialect.h"
#include "mlir/IR/Builders.h"
#include "mlir/IR/BuiltinTypes.h"
#include "mlir/IR/OpImplementation.h"
#include "mlir/IR/PatternMatch.h"
#include "mlir/IR/Region.h"
#include "mlir/Transforms/InliningUtils.h"

using namespace mlir;

namespace eda {

//===----------------------------------------------------------------------===//
// Variable Operations
//===----------------------------------------------------------------------===//

ParseResult AssignOp::parse(OpAsmParser &parser, OperationState &result) {
  StringAttr nameAttr;
  Value value;

  if (parser.parseLParen())
    return failure();

  if (parser.parseAttribute(nameAttr))
    return failure();

  if (parser.parseComma())
    return failure();

  if (parser.parseOperand(value))
    return failure();

  if (parser.parseRParen())
    return failure();

  if (parser.parseOptionalAttrDict(result.attributes))
    return failure();

  // Parse the type
  Type resultType;
  if (parser.parseColon())
    return failure();

  if (parser.parseType(resultType))
    return failure();

  result.addOperands(value);
  result.addTypes(resultType);
  result.addAttribute("name", nameAttr);

  return success();
}

void AssignOp::print(OpAsmPrinter &p) {
  p << " assign(";
  p.printAttributeWithoutType(getNameAttr());
  p << ", ";
  p.printOperand(getValue());
  p << ") : (" << getValue().getType() << ") -> " << getType();
}

LogicalResult AssignOp::verify() {
  // The result type should match the value type
  if (getValue().getType() != getType())
    return emitOpError("result type must match value type");
  return success();
}

ParseResult LoadOp::parse(OpAsmParser &parser, OperationState &result) {
  StringAttr nameAttr;

  if (parser.parseLParen())
    return failure();

  if (parser.parseAttribute(nameAttr))
    return failure();

  if (parser.parseRParen())
    return failure();

  if (parser.parseOptionalAttrDict(result.attributes))
    return failure();

  // Parse the result type
  Type resultType;
  if (parser.parseColon())
    return failure();

  if (parser.parseType(resultType))
    return failure();

  result.addTypes(resultType);
  result.addAttribute("name", nameAttr);

  return success();
}

void LoadOp::print(OpAsmPrinter &p) {
  p << " load(";
  p.printAttributeWithoutType(getNameAttr());
  p << ") : () -> " << getType();
}

ParseResult StoreOp::parse(OpAsmParser &parser, OperationState &result) {
  StringAttr nameAttr;
  Value value;

  if (parser.parseLParen())
    return failure();

  if (parser.parseAttribute(nameAttr))
    return failure();

  if (parser.parseComma())
    return failure();

  if (parser.parseOperand(value))
    return failure();

  if (parser.parseRParen())
    return failure();

  if (parser.parseOptionalAttrDict(result.attributes))
    return failure();

  // Parse the type signature
  FunctionType type;
  if (parser.parseColon())
    return failure();

  if (parser.parseType(type))
    return failure();

  result.addOperands(value);
  result.addAttribute("name", nameAttr);

  return success();
}

void StoreOp::print(OpAsmPrinter &p) {
  p << " store(";
  p.printAttributeWithoutType(getNameAttr());
  p << ", ";
  p.printOperand(getValue());
  p << ") : (" << getValue().getType() << ") -> ()";
}

//===----------------------------------------------------------------------===//
// Arithmetic Operations
//===----------------------------------------------------------------------===//

LogicalResult AddOp::inferReturnTypes(
    MLIRContext *context, std::optional<Location> location,
    ValueRange operands, DictionaryAttr attributes,
    OpaqueProperties properties, RegionRange regions,
    SmallVectorImpl<Type> &inferredReturnTypes) {
  if (operands.size() != 2)
    return failure();

  // Result type is the same as the operand type
  inferredReturnTypes.push_back(operands[0].getType());
  return success();
}

LogicalResult SubOp::inferReturnTypes(
    MLIRContext *context, std::optional<Location> location,
    ValueRange operands, DictionaryAttr attributes,
    OpaqueProperties properties, RegionRange regions,
    SmallVectorImpl<Type> &inferredReturnTypes) {
  if (operands.size() != 2)
    return failure();

  inferredReturnTypes.push_back(operands[0].getType());
  return success();
}

LogicalResult MulOp::inferReturnTypes(
    MLIRContext *context, std::optional<Location> location,
    ValueRange operands, DictionaryAttr attributes,
    OpaqueProperties properties, RegionRange regions,
    SmallVectorImpl<Type> &inferredReturnTypes) {
  if (operands.size() != 2)
    return failure();

  inferredReturnTypes.push_back(operands[0].getType());
  return success();
}

LogicalResult DivOp::inferReturnTypes(
    MLIRContext *context, std::optional<Location> location,
    ValueRange operands, DictionaryAttr attributes,
    OpaqueProperties properties, RegionRange regions,
    SmallVectorImpl<Type> &inferredReturnTypes) {
  if (operands.size() != 2)
    return failure();

  inferredReturnTypes.push_back(operands[0].getType());
  return success();
}

//===----------------------------------------------------------------------===//
// Comparison Operations
//===----------------------------------------------------------------------===//

LogicalResult CmpEqOp::inferReturnTypes(
    MLIRContext *context, std::optional<Location> location,
    ValueRange operands, DictionaryAttr attributes,
    OpaqueProperties properties, RegionRange regions,
    SmallVectorImpl<Type> &inferredReturnTypes) {
  if (operands.size() != 2)
    return failure();

  inferredReturnTypes.push_back(IntegerType::get(context, 1));
  return success();
}

LogicalResult CmpGtOp::inferReturnTypes(
    MLIRContext *context, std::optional<Location> location,
    ValueRange operands, DictionaryAttr attributes,
    OpaqueProperties properties, RegionRange regions,
    SmallVectorImpl<Type> &inferredReturnTypes) {
  if (operands.size() != 2)
    return failure();

  inferredReturnTypes.push_back(IntegerType::get(context, 1));
  return success();
}

LogicalResult CmpLtOp::inferReturnTypes(
    MLIRContext *context, std::optional<Location> location,
    ValueRange operands, DictionaryAttr attributes,
    OpaqueProperties properties, RegionRange regions,
    SmallVectorImpl<Type> &inferredReturnTypes) {
  if (operands.size() != 2)
    return failure();

  inferredReturnTypes.push_back(IntegerType::get(context, 1));
  return success();
}

//===----------------------------------------------------------------------===//
// Logical Operations
//===----------------------------------------------------------------------===//

LogicalResult AndOp::inferReturnTypes(
    MLIRContext *context, std::optional<Location> location,
    ValueRange operands, DictionaryAttr attributes,
    OpaqueProperties properties, RegionRange regions,
    SmallVectorImpl<Type> &inferredReturnTypes) {
  if (operands.size() != 2)
    return failure();

  inferredReturnTypes.push_back(IntegerType::get(context, 1));
  return success();
}

LogicalResult OrOp::inferReturnTypes(
    MLIRContext *context, std::optional<Location> location,
    ValueRange operands, DictionaryAttr attributes,
    OpaqueProperties properties, RegionRange regions,
    SmallVectorImpl<Type> &inferredReturnTypes) {
  if (operands.size() != 2)
    return failure();

  inferredReturnTypes.push_back(IntegerType::get(context, 1));
  return success();
}

LogicalResult NotOp::inferReturnTypes(
    MLIRContext *context, std::optional<Location> location,
    ValueRange operands, DictionaryAttr attributes,
    OpaqueProperties properties, RegionRange regions,
    SmallVectorImpl<Type> &inferredReturnTypes) {
  if (operands.size() != 1)
    return failure();

  inferredReturnTypes.push_back(IntegerType::get(context, 1));
  return success();
}

//===----------------------------------------------------------------------===//
// System Operations
//===----------------------------------------------------------------------===//

LogicalResult PutsOp::verify() {
  // Puts op should have at least one argument
  if (getArgs().empty())
    return emitOpError("puts requires at least one argument");
  return success();
}

LogicalResult FormatOp::inferReturnTypes(
    MLIRContext *context, std::optional<Location> location,
    ValueRange operands, DictionaryAttr attributes,
    OpaqueProperties properties, RegionRange regions,
    SmallVectorImpl<Type> &inferredReturnTypes) {
  if (operands.empty())
    return failure();

  // Return type is always a string
  inferredReturnTypes.push_back(StringType::get(context));
  return success();
}

LogicalResult ConcatOp::inferReturnTypes(
    MLIRContext *context, std::optional<Location> location,
    ValueRange operands, DictionaryAttr attributes,
    OpaqueProperties properties, RegionRange regions,
    SmallVectorImpl<Type> &inferredReturnTypes) {
  // Return type is always a string
  inferredReturnTypes.push_back(StringType::get(context));
  return success();
}

//===----------------------------------------------------------------------===//
// Function Operations
//===----------------------------------------------------------------------===//

FuncOp CallOp::getCalleeOp() {
  SymbolTableAttr symbolTable = (*this)->getParentOfType<ModuleOp>()
                                   .getSymbolTable();
  return symbolTable.lookup<FuncOp>(getCalleeAttr());
}

void ReturnOp::build(OpBuilder &builder, OperationState &result,
                     ValueRange operands) {
  result.addOperands(operands);
}

//===----------------------------------------------------------------------===//
// File System Operations
//===----------------------------------------------------------------------===//

LogicalResult FileExistsOp::inferReturnTypes(
    MLIRContext *context, std::optional<Location> location,
    ValueRange operands, DictionaryAttr attributes,
    OpaqueProperties properties, RegionRange regions,
    SmallVectorImpl<Type> &inferredReturnTypes) {
  if (operands.size() != 1)
    return failure();

  inferredReturnTypes.push_back(IntegerType::get(context, 1));
  return success();
}

LogicalResult FileReadOp::inferReturnTypes(
    MLIRContext *context, std::optional<Location> location,
    ValueRange operands, DictionaryAttr attributes,
    OpaqueProperties properties, RegionRange regions,
    SmallVectorImpl<Type> &inferredReturnTypes) {
  if (operands.size() != 1)
    return failure();

  inferredReturnTypes.push_back(StringType::get(context));
  return success();
}

LogicalResult FileGlobOp::inferReturnTypes(
    MLIRContext *context, std::optional<Location> location,
    ValueRange operands, DictionaryAttr attributes,
    OpaqueProperties properties, RegionRange regions,
    SmallVectorImpl<Type> &inferredReturnTypes) {
  if (operands.size() != 1)
    return failure();

  inferredReturnTypes.push_back(ListType::get(StringType::get(context)));
  return success();
}

//===----------------------------------------------------------------------===//
// Constant Operations
//===----------------------------------------------------------------------===//

OpFoldResult ConstantOp::fold(FoldAdaptor adaptor) {
  return getValue();
}

} // namespace eda

#define GET_OP_CLASSES
#include "eda-dialect/EDAOps.cpp.inc"