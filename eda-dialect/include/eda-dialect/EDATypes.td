//===-- EDATypes.td - EDA type definitions ------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef EDA_TYPES
#define EDA_TYPES

include "mlir/IR/AttrTypeBase.td"
include "mlir/Interfaces/DataLayoutInterfaces.td"
include "eda-dialect/EDADialect.td"

//===----------------------------------------------------------------------===//
// TCL String Type
//===----------------------------------------------------------------------===//

class EDA_StringType<string name> : TypeDef<EDA_Dialect, name> {
  let mnemonic = "tcl_string";

  let summary = "TCL string type";
  let description = [{
    Represents a string value in TCL, which can contain any sequence of characters.
  }];

  let parameters = (ins);
}

def StringType : EDA_StringType<"String">;

//===----------------------------------------------------------------------===//
// TCL List Type
//===----------------------------------------------------------------------===//

class EDA_ListType<string name> : TypeDef<EDA_Dialect, name> {
  let mnemonic = "tcl_list";

  let summary = "TCL list type";
  let description = [{
    Represents a list in TCL, which is an ordered collection of elements.
    The element type can be any supported type.
  }];

  let parameters = (ins
    "Type" : $elementType
  );

  let assemblyFormat = "`<` $elementType `>`";
}

def ListType : EDA_ListType<"List">;

//===----------------------------------------------------------------------===//
// TCL Dict Type
//===----------------------------------------------------------------------===//

class EDA_DictType<string name> : TypeDef<EDA_Dialect, name> {
  let mnemonic = "tcl_dict";

  let summary = "TCL dictionary type";
  let description = [{
    Represents a dictionary in TCL, which maps keys to values.
    Both keys and values can be of any supported type.
  }];

  let parameters = (ins
    "Type" : $keyType,
    "Type" : $valueType
  );

  let assemblyFormat = "`<` $keyType `,` $valueType `>`";
}

def DictType : EDA_DictType<"Dict">;

//===----------------------------------------------------------------------===//
// TCL Variable Type
//===----------------------------------------------------------------------===//

class EDA_VariableType<string name> : TypeDef<EDA_Dialect, name> {
  let mnemonic = "tcl_variable";

  let summary = "TCL variable type";
  let description = [{
    Represents a variable in TCL that can hold a value of a specific type.
    This is used to model variable assignments and references.
  }];

  let parameters = (ins
    "Type" : $containedType
  );

  let assemblyFormat = "`<` $containedType `>`";
}

def VariableType : EDA_VariableType<"Variable">;

//===----------------------------------------------------------------------===//
// TCL Any Type
//===----------------------------------------------------------------------===//

class EDA_AnyType<string name> : TypeDef<EDA_Dialect, name> {
  let mnemonic = "tcl_any";

  let summary = "TCL any type";
  let description = [{
    Represents a value that can be of any type in TCL.
    This is used for type-erased operations and dynamic typing.
  }];

  let parameters = (ins);
}

def EDAAnyType : EDA_AnyType<"Any">;

#endif // EDA_TYPES