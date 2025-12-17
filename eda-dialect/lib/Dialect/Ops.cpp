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
#include "mlir/IR/BuiltinOps.h"
#include "mlir/IR/BuiltinTypes.h"
#include "mlir/IR/OpImplementation.h"
#include "mlir/IR/PatternMatch.h"
#include "mlir/IR/Region.h"
#include "mlir/IR/SymbolTable.h"
#include "mlir/Transforms/InliningUtils.h"

using namespace mlir;

namespace eda {

//===----------------------------------------------------------------------===//
// Variable Operations
//===----------------------------------------------------------------------===//

ParseResult AssignOp::parse(OpAsmParser &parser, OperationState &result) {
  StringAttr nameAttr;
  OpAsmParser::UnresolvedOperand value;

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
  Type valueType, resultType;
  if (parser.parseColon() || parser.parseLParen() || parser.parseType(valueType) ||
      parser.parseRParen() || parser.parseArrow() || parser.parseType(resultType))
    return failure();

  if (parser.resolveOperand(value, valueType, result.operands))
    return failure();

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
  OpAsmParser::UnresolvedOperand value;

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
  Type valueType;
  if (parser.parseColon() || parser.parseLParen() || parser.parseType(valueType) ||
      parser.parseRParen() || parser.parseArrow())
    return failure();

  if (parser.resolveOperand(value, valueType, result.operands))
    return failure();

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
// System Operations
//===----------------------------------------------------------------------===//

LogicalResult PutsOp::verify() {
  // Puts op should have at least one argument
  if (getArgs().empty())
    return emitOpError("puts requires at least one argument");
  return success();
}

//===----------------------------------------------------------------------===//
// Function Operations
//===----------------------------------------------------------------------===//

FuncOp CallOp::getCalleeOp() {
  return SymbolTable::lookupNearestSymbolFrom<FuncOp>(*this, getCalleeAttr());
}

//===----------------------------------------------------------------------===//
// Builder implementations for custom signatures
//===----------------------------------------------------------------------===//

void AssignOp::build(OpBuilder &builder, OperationState &state,
                     StringRef name, Value value) {
  state.addAttribute("name", builder.getStringAttr(name));
  state.addOperands(value);
  state.addTypes(value.getType());
}

void StoreOp::build(OpBuilder &builder, OperationState &state,
                    StringRef name, Value value) {
  state.addAttribute("name", builder.getStringAttr(name));
  state.addOperands(value);
}

void ConstantOp::build(OpBuilder &builder, OperationState &state,
                       Attribute value) {
  auto valueAttr = llvm::dyn_cast<StringAttr>(value);
  assert(valueAttr && "value must be a StringAttr");

  auto stringType = StringType::get(builder.getContext());
  state.addAttribute("value", valueAttr);
  state.addTypes(stringType);
}

void YieldOp::build(OpBuilder &builder, OperationState &state) {
  // YieldOp has no operands, attributes, or results
}

//===----------------------------------------------------------------------===//
// Constant Operations
//===----------------------------------------------------------------------===//

OpFoldResult ConstantOp::fold(FoldAdaptor adaptor) {
  return getValue();
}

} // namespace eda

//===----------------------------------------------------------------------===//
// InferTypeOpInterface implementations
//===----------------------------------------------------------------------===//

// Arithmetic operations infer result types from operands
mlir::LogicalResult eda::AddOp::inferReturnTypes(
    mlir::MLIRContext *context, std::optional<mlir::Location> location,
    mlir::ValueRange operands, mlir::DictionaryAttr attributes,
    mlir::OpaqueProperties properties, mlir::RegionRange regions,
    llvm::SmallVectorImpl<mlir::Type> &inferredReturnTypes) {
  if (operands.empty())
    return mlir::failure();
  inferredReturnTypes.push_back(operands[0].getType());
  return mlir::success();
}

mlir::LogicalResult eda::SubOp::inferReturnTypes(
    mlir::MLIRContext *context, std::optional<mlir::Location> location,
    mlir::ValueRange operands, mlir::DictionaryAttr attributes,
    mlir::OpaqueProperties properties, mlir::RegionRange regions,
    llvm::SmallVectorImpl<mlir::Type> &inferredReturnTypes) {
  if (operands.empty())
    return mlir::failure();
  inferredReturnTypes.push_back(operands[0].getType());
  return mlir::success();
}

mlir::LogicalResult eda::MulOp::inferReturnTypes(
    mlir::MLIRContext *context, std::optional<mlir::Location> location,
    mlir::ValueRange operands, mlir::DictionaryAttr attributes,
    mlir::OpaqueProperties properties, mlir::RegionRange regions,
    llvm::SmallVectorImpl<mlir::Type> &inferredReturnTypes) {
  if (operands.empty())
    return mlir::failure();
  inferredReturnTypes.push_back(operands[0].getType());
  return mlir::success();
}

mlir::LogicalResult eda::DivOp::inferReturnTypes(
    mlir::MLIRContext *context, std::optional<mlir::Location> location,
    mlir::ValueRange operands, mlir::DictionaryAttr attributes,
    mlir::OpaqueProperties properties, mlir::RegionRange regions,
    llvm::SmallVectorImpl<mlir::Type> &inferredReturnTypes) {
  if (operands.empty())
    return mlir::failure();
  inferredReturnTypes.push_back(operands[0].getType());
  return mlir::success();
}

// File operations return ListType
mlir::LogicalResult eda::FileGlobOp::inferReturnTypes(
    mlir::MLIRContext *context, std::optional<mlir::Location> location,
    mlir::ValueRange operands, mlir::DictionaryAttr attributes,
    mlir::OpaqueProperties properties, mlir::RegionRange regions,
    llvm::SmallVectorImpl<mlir::Type> &inferredReturnTypes) {
  auto stringType = eda::StringType::get(context);
  inferredReturnTypes.push_back(eda::ListType::get(context, stringType));
  return mlir::success();
}

//===----------------------------------------------------------------------===//
// LoopLikeOpInterface implementations
//===----------------------------------------------------------------------===//

llvm::SmallVector<mlir::Region *> eda::ForOp::getLoopRegions() {
  llvm::SmallVector<mlir::Region *> regions;
  regions.push_back(&getBody());
  return regions;
}

llvm::SmallVector<mlir::Region *> eda::WhileOp::getLoopRegions() {
  llvm::SmallVector<mlir::Region *> regions;
  regions.push_back(&getCond());
  regions.push_back(&getBody());
  return regions;
}

//===----------------------------------------------------------------------===//
// RegionBranchOpInterface implementations
//===----------------------------------------------------------------------===//

void eda::IfOp::getSuccessorRegions(mlir::RegionBranchPoint point,
                                     llvm::SmallVectorImpl<mlir::RegionSuccessor> &regions) {
  // The then and else regions branch back to the parent operation
  if (!point.isParent()) {
    regions.push_back(mlir::RegionSuccessor(getResults()));
    return;
  }

  // If the condition is constant, we can give a more precise answer
  regions.push_back(mlir::RegionSuccessor(&getThenRegion()));
  regions.push_back(mlir::RegionSuccessor(&getElseRegion()));
}

// Include TableGen generated op definitions
#define GET_OP_CLASSES
#include "eda-dialect/EDAOps.cpp.inc"