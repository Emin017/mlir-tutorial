//===-- EDAOps.td - EDA operation definitions ----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef EDA_OPS
#define EDA_OPS

include "mlir/IR/OpBase.td"
include "mlir/IR/SymbolInterfaces.td"
include "mlir/Interfaces/ControlFlowInterfaces.td"
include "mlir/Interfaces/FunctionInterfaces.td"
include "mlir/Interfaces/InferTypeOpInterface.td"
include "mlir/Interfaces/LoopLikeInterface.td"
include "mlir/Interfaces/SideEffectInterfaces.td"

include "eda-dialect/EDADialect.td"
include "eda-dialect/EDATypes.td"

//===----------------------------------------------------------------------===//
// Variable Operations
//===----------------------------------------------------------------------===//

class EDA_VariableOp<string mnemonic, list<Trait> traits = []> :
    Op<EDA_Dialect, mnemonic, traits> {
}

def AssignOp : EDA_VariableOp<"assign"> {
  let summary = "Assign a value to a variable";
  let description = [{
    Assigns a value to a TCL variable. The variable name is provided as a
    string attribute, and the value can be of any supported type.
  }];

  let arguments = (ins
    StrAttr:$name,
    AnyType:$value
  );

  let results = (outs
    AnyType:$result
  );

  let builders = [
    OpBuilder<(ins "::llvm::StringRef":$name, "::mlir::Value":$value)>
  ];

  let hasCustomAssemblyFormat = 1;
  let hasVerifier = 1;
}

def LoadOp : EDA_VariableOp<"load"> {
  let summary = "Load a value from a variable";
  let description = [{
    Loads the current value of a TCL variable. The variable name is provided
    as a string attribute.
  }];

  let arguments = (ins
    StrAttr:$name
  );

  let results = (outs
    AnyType:$result
  );

  let builders = [
    OpBuilder<(ins "::llvm::StringRef":$name)>
  ];

  let hasCustomAssemblyFormat = 1;
}

def StoreOp : EDA_VariableOp<"store"> {
  let summary = "Store a value to a variable";
  let description = [{
    Stores a value to an existing TCL variable. The variable name is provided
    as a string attribute, and the value can be of any supported type.
  }];

  let arguments = (ins
    StrAttr:$name,
    AnyType:$value
  );

  let results = (outs);

  let builders = [
    OpBuilder<(ins "::llvm::StringRef":$name, "::mlir::Value":$value)>
  ];

  let hasCustomAssemblyFormat = 1;
}

//===----------------------------------------------------------------------===//
// Arithmetic Operations
//===----------------------------------------------------------------------===//

class EDA_ArithOp<string mnemonic> :
    Op<EDA_Dialect, mnemonic, [Commutative, DeclareOpInterfaceMethods<InferTypeOpInterface>]> {
  let summary = "Arithmetic " # mnemonic # " operation";
  let assemblyFormat = "`(` $lhs `,` $rhs `)` attr-dict `:` `(` type($lhs) `,` type($rhs) `)` `->` type($result)";
}

def AddOp : EDA_ArithOp<"add"> {
  let description = [{
    Performs addition on two values. Supports integer, floating-point,
    and string concatenation.
  }];

  let arguments = (ins
    AnyType:$lhs,
    AnyType:$rhs
  );

  let results = (outs
    AnyType:$result
  );

}

def SubOp : EDA_ArithOp<"sub"> {
  let description = [{
    Performs subtraction on two numeric values.
  }];

  let arguments = (ins
    AnyType:$lhs,
    AnyType:$rhs
  );

  let results = (outs
    AnyType:$result
  );
}

def MulOp : EDA_ArithOp<"mul"> {
  let description = [{
    Performs multiplication on two numeric values.
  }];

  let arguments = (ins
    AnyType:$lhs,
    AnyType:$rhs
  );

  let results = (outs
    AnyType:$result
  );
}

def DivOp : EDA_ArithOp<"div"> {
  let description = [{
    Performs division on two numeric values.
  }];

  let arguments = (ins
    AnyType:$lhs,
    AnyType:$rhs
  );

  let results = (outs
    AnyType:$result
  );
}

//===----------------------------------------------------------------------===//
// Comparison Operations
//===----------------------------------------------------------------------===//

class EDA_CmpOp<string mnemonic> : Op<EDA_Dialect, mnemonic, [DeclareOpInterfaceMethods<InferTypeOpInterface>]> {
  let summary = "Comparison " # mnemonic # " operation";
  let assemblyFormat = "`(` $lhs `,` $rhs `)` attr-dict `:` `(` type($lhs) `,` type($rhs) `)` `->` type($result)";
}

def CmpEqOp : EDA_CmpOp<"cmp_eq"> {
  let description = [{
    Compares two values for equality.
  }];

  let arguments = (ins
    AnyType:$lhs,
    AnyType:$rhs
  );

  let results = (outs
    I1:$result
  );
}

def CmpGtOp : EDA_CmpOp<"cmp_gt"> {
  let description = [{
    Compares if the left value is greater than the right value.
  }];

  let arguments = (ins
    AnyType:$lhs,
    AnyType:$rhs
  );

  let results = (outs
    I1:$result
  );
}

def CmpLtOp : EDA_CmpOp<"cmp_lt"> {
  let description = [{
    Compares if the left value is less than the right value.
  }];

  let arguments = (ins
    AnyType:$lhs,
    AnyType:$rhs
  );

  let results = (outs
    I1:$result
  );
}

//===----------------------------------------------------------------------===//
// Logical Operations
//===----------------------------------------------------------------------===//

def AndOp : Op<EDA_Dialect, "and", [Commutative, DeclareOpInterfaceMethods<InferTypeOpInterface>]> {
  let summary = "Logical AND operation";
  let description = [{
    Performs logical AND on two boolean values.
  }];

  let arguments = (ins
    I1:$lhs,
    I1:$rhs
  );

  let results = (outs
    I1:$result
  );

  let assemblyFormat = "`(` $lhs `,` $rhs `)` attr-dict `:` type($result)";
}

def OrOp : Op<EDA_Dialect, "or", [Commutative, DeclareOpInterfaceMethods<InferTypeOpInterface>]> {
  let summary = "Logical OR operation";
  let description = [{
    Performs logical OR on two boolean values.
  }];

  let arguments = (ins
    I1:$lhs,
    I1:$rhs
  );

  let results = (outs
    I1:$result
  );

  let assemblyFormat = "`(` $lhs `,` $rhs `)` attr-dict `:` type($result)";
}

def NotOp : Op<EDA_Dialect, "not", [DeclareOpInterfaceMethods<InferTypeOpInterface>]> {
  let summary = "Logical NOT operation";
  let description = [{
    Performs logical NOT on a boolean value.
  }];

  let arguments = (ins
    I1:$input
  );

  let results = (outs
    I1:$result
  );

  let assemblyFormat = "`(` $input `)` attr-dict `:` type($result)";
}

//===----------------------------------------------------------------------===//
// System Operations
//===----------------------------------------------------------------------===//

def PutsOp : Op<EDA_Dialect, "puts"> {
  let summary = "Print a message to stdout";
  let description = [{
    Prints a message to stdout, similar to TCL's puts command.
    Supports format strings with arguments.
  }];

  let arguments = (ins
    Variadic<AnyType>:$args
  );

  let results = (outs);

  let assemblyFormat = "`(` $args `)` attr-dict `:` functional-type($args, results)";

  let hasVerifier = 1;
}

def FormatOp : Op<EDA_Dialect, "format", [DeclareOpInterfaceMethods<InferTypeOpInterface>]> {
  let summary = "Format a string";
  let description = [{
    Formats a string using a format pattern and arguments.
  }];

  let arguments = (ins
    StringType:$pattern,
    Variadic<AnyType>:$args
  );

  let results = (outs
    StringType:$result
  );

  let assemblyFormat = "`(` $pattern `,` $args `)` attr-dict `:` functional-type($args, results)";
}

def ConcatOp : Op<EDA_Dialect, "concat", [Commutative, DeclareOpInterfaceMethods<InferTypeOpInterface>]> {
  let summary = "Concatenate strings";
  let description = [{
    Concatenates multiple strings into one.
  }];

  let arguments = (ins
    Variadic<AnyType>:$strings
  );

  let results = (outs
    StringType:$result
  );

  let assemblyFormat = "`(` $strings `)` attr-dict `:` functional-type($strings, results)";
}

//===----------------------------------------------------------------------===//
// Control Flow Operations
//===----------------------------------------------------------------------===//

def IfOp : Op<EDA_Dialect, "if", [
  DeclareOpInterfaceMethods<RegionBranchOpInterface>,
  NoRegionArguments,
  SingleBlockImplicitTerminator<"eda::YieldOp">
]> {
  let summary = "If-then-else operation";
  let description = [{
    The `eda.if` operation represents an if-then-else construct for
    conditional execution in TCL scripts.
  }];

  let arguments = (ins
    I1:$condition
  );

  let results = (outs
    Variadic<AnyType>:$results
  );

  let regions = (region
    SizedRegion<1>:$thenRegion,
    AnyRegion:$elseRegion
  );

  let assemblyFormat = [{
    $condition `,` $thenRegion (`:` $elseRegion^)? attr-dict `:` functional-type($condition, $results)
  }];

  let builders = [
    OpBuilder<(ins "::mlir::Value":$condition, "::mlir::TypeRange":$resultTypes)>
  ];
}

def ForOp : Op<EDA_Dialect, "for", [
  DeclareOpInterfaceMethods<LoopLikeOpInterface>,
  SingleBlockImplicitTerminator<"eda::YieldOp">
]> {
  let summary = "For loop operation";
  let description = [{
    The `eda.for` operation represents a for loop that iterates from
    a start value to an end value with an optional step.
  }];

  let arguments = (ins
    AnyType:$start,
    AnyType:$end,
    AnyType:$step
  );

  let results = (outs);

  let regions = (region
    SizedRegion<1>:$body
  );

  let assemblyFormat = "operands attr-dict `:` type(operands) $body";
}

def WhileOp : Op<EDA_Dialect, "while", [
  DeclareOpInterfaceMethods<LoopLikeOpInterface>,
  SingleBlockImplicitTerminator<"eda::YieldOp">
]> {
  let summary = "While loop operation";
  let description = [{
    The `eda.while` operation represents a while loop that continues
    as long as the condition is true.
  }];

  let arguments = (ins);

  let results = (outs);

  let regions = (region
    SizedRegion<1>:$cond,
    SizedRegion<1>:$body
  );

  let assemblyFormat = "`(` $cond `)` `,` $body attr-dict";
}

def YieldOp : Op<EDA_Dialect, "yield", [
  Pure,
  ReturnLike,
  Terminator
]> {
  let summary = "Yield operation";
  let description = [{
    The `eda.yield` operation yields values from a region and is used to
    terminate the blocks inside control flow operations.
  }];

  let arguments = (ins
    Variadic<AnyType>:$operands
  );

  let assemblyFormat = "`(` $operands `)` attr-dict `:` type($operands)";

  let builders = [
    OpBuilder<(ins)>
  ];
}

def BreakOp : Op<EDA_Dialect, "break", [Terminator]> {
  let summary = "Break operation";
  let description = [{
    The `eda.break` operation breaks out of the enclosing loop.
  }];

  let arguments = (ins);

  let assemblyFormat = "attr-dict";
}

def ContinueOp : Op<EDA_Dialect, "continue", [Terminator]> {
  let summary = "Continue operation";
  let description = [{
    The `eda.continue` operation continues to the next iteration of
    the enclosing loop.
  }];

  let arguments = (ins);

  let assemblyFormat = "attr-dict";
}

//===----------------------------------------------------------------------===//
// Function Operations
//===----------------------------------------------------------------------===//

def FuncOp : Op<EDA_Dialect, "func", [
  AutomaticAllocationScope,
  IsolatedFromAbove,
  Symbol
]> {
  let summary = "Function operation";
  let description = [{
    The `eda.func` operation represents a function definition that can
    be called from other parts of the program.
  }];

  let arguments = (ins
    TypeAttrOf<FunctionType>:$function_type,
    StrAttr:$sym_name
  );

  let results = (outs);

  let regions = (region
    SizedRegion<1>:$body
  );

  let builders = [
    OpBuilder<(ins "::llvm::StringRef":$name, "::mlir::FunctionType":$type)>
  ];

  let extraClassDeclaration = [{
    /// Returns the argument types of this function.
    ::llvm::ArrayRef<::mlir::Type> getArgumentTypes() { return getFunctionType().getInputs(); }

    /// Returns the result types of this function.
    ::llvm::ArrayRef<::mlir::Type> getResultTypes() { return getFunctionType().getResults(); }
  }];
}

def CallOp : Op<EDA_Dialect, "call", [
  MemoryEffects<[]>
]> {
  let summary = "Call operation";
  let description = [{
    The `eda.call` operation calls a function with the given arguments.
  }];

  let arguments = (ins
    FlatSymbolRefAttr:$callee,
    Variadic<AnyType>:$operands
  );

  let results = (outs
    Variadic<AnyType>:$results
  );

  let assemblyFormat = [{
    $callee `(` $operands `)` attr-dict `:` functional-type($operands, $results)
  }];

  let builders = [
    OpBuilder<(ins "::eda::FuncOp":$callee, "::mlir::ValueRange":$operands)>
  ];

  let extraClassDeclaration = [{
    /// Get the callee function.
    FuncOp getCalleeOp();
  }];
}

def ReturnOp : Op<EDA_Dialect, "return", [
  ReturnLike,
  Terminator,
  HasParent<"eda::FuncOp">
]> {
  let summary = "Return operation";
  let description = [{
    The `eda.return` operation returns from a function with the given values.
  }];

  let arguments = (ins
    Variadic<AnyType>:$operands
  );

  let assemblyFormat = "attr-dict (`(` $operands^ `:` type($operands) `)`)?";
}

//===----------------------------------------------------------------------===//
// File System Operations
//===----------------------------------------------------------------------===//

def FileExistsOp : Op<EDA_Dialect, "file_exists", [Pure, DeclareOpInterfaceMethods<InferTypeOpInterface>]> {
  let summary = "Check if file exists";
  let description = [{
    Checks if a file exists at the given path.
  }];

  let arguments = (ins
    StringType:$path
  );

  let results = (outs
    I1:$result
  );

  let assemblyFormat = "`(` $path `)` attr-dict `:` `(` type($path) `)` `->` type($result)";
}

def FileReadOp : Op<EDA_Dialect, "file_read", [MemoryEffects<[MemRead]>, DeclareOpInterfaceMethods<InferTypeOpInterface>]> {
  let summary = "Read file contents";
  let description = [{
    Reads the entire contents of a file into a string.
  }];

  let arguments = (ins
    StringType:$path
  );

  let results = (outs
    StringType:$content
  );

  let assemblyFormat = "`(` $path `)` attr-dict `:` `(` type($path) `)` `->` type($content)";
}

def FileWriteOp : Op<EDA_Dialect, "file_write", [MemoryEffects<[MemWrite]>]> {
  let summary = "Write contents to file";
  let description = [{
    Writes string contents to a file.
  }];

  let arguments = (ins
    StringType:$path,
    StringType:$content
  );

  let results = (outs);

  let assemblyFormat = "`(` $path `,` $content `)` attr-dict `:` functional-type($path, $content)";
}

def FileGlobOp : Op<EDA_Dialect, "file_glob", [Pure, DeclareOpInterfaceMethods<InferTypeOpInterface>]> {
  let summary = "Glob pattern matching";
  let description = [{
    Finds files matching a glob pattern.
  }];

  let arguments = (ins
    StringType:$pattern
  );

  let results = (outs
    ListType:$result
  );

  let assemblyFormat = "`(` $pattern `)` attr-dict `:` `(` type($pattern) `)` `->` type($result)";
}

//===----------------------------------------------------------------------===//
// Constant Operations
//===----------------------------------------------------------------------===//

def ConstantOp : Op<EDA_Dialect, "constant", [ConstantLike, Pure]> {
  let summary = "Constant operation";
  let description = [{
    Defines a constant value.
  }];

  let arguments = (ins
    AnyAttr:$value
  );

  let results = (outs
    AnyType:$result
  );

  let assemblyFormat = "`(` $value `)` attr-dict `:` type($result)";

  let builders = [
    OpBuilder<(ins "::mlir::Attribute":$value)>
  ];

  let hasFolder = 1;
}

#endif // EDA_OPS