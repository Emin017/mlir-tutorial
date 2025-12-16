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
#include "mlir/IR/Types.h"

// Generated headers
#include "eda-dialect/EDADialect.h.inc"

namespace eda {

/// The EDA dialect provides operations for generating TCL scripts.
class EDADialect : public mlir::Dialect {
public:
  explicit EDADialect(mlir::MLIRContext *context);

  /// Get the singleton instance of the EDA dialect.
  static EDADialect &get(mlir::MLIRContext *context) {
    return *context->getLoadedDialect<EDADialect>();
  }

  /// Parse a type registered to this dialect.
  mlir::Type parseType(mlir::DialectAsmParser &parser) const override;

  /// Print a type registered to this dialect.
  void printType(mlir::Type type,
                 mlir::DialectAsmPrinter &printer) const override;

private:
  /// Register the types of this dialect.
  void registerTypes();
};

} // namespace eda

#endif // EDA_DIALECT_H