//===-- EDADialect.td - EDA dialect definition ---------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef EDA_DIALECT
#define EDA_DIALECT

include "mlir/IR/DialectBase.td"

def EDA_Dialect : Dialect {
  let name = "eda";
  let cppNamespace = "eda";

  let summary = "EDA dialect for TCL code generation";
  let description = [{
    The EDA dialect provides operations for generating TCL scripts commonly
    used in Electronic Design Automation (EDA) tools. It supports type-safe
    variable operations, arithmetic expressions, control flow structures,
    and file system operations.
  }];

  let dependentDialects = [];
  let useDefaultTypePrinterParser = 0;
  let useDefaultAttributePrinterParser = 0;

  let extraClassDeclaration = [{
    /// Parse a type registered to this dialect.
    mlir::Type parseType(mlir::DialectAsmParser &parser) const override;

    /// Print a type registered to this dialect.
    void printType(mlir::Type type, mlir::DialectAsmPrinter &os) const override;

    /// Parse an attribute registered to this dialect.
    mlir::Attribute parseAttribute(mlir::DialectAsmParser &parser,
                                     mlir::Type type) const override;

    /// Print an attribute registered to this dialect.
    void printAttribute(mlir::Attribute attr,
                         mlir::DialectAsmPrinter &os) const override;
  }];
}

#endif // EDA_DIALECT