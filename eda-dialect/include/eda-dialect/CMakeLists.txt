# EDA Dialect TableGen 配置

# 设置 TableGen 输入文件
set(EDA_DIALECT_TD_FILES
  EDADialect.td
  EDAOps.td
  EDATypes.td
)

# 生成 Dialect 头文件
set(LLVM_TARGET_DEFINITIONS EDADialect.td)
mlir_tablegen(EDADialect.h.inc -gen-dialect-decls)
mlir_tablegen(EDADialect.cpp.inc -gen-dialect-defs)
add_public_tablegen_target(MLIREDAEDialectIncGen)

# 生成 Operations
set(LLVM_TARGET_DEFINITIONS EDAOps.td)
mlir_tablegen(EDAOps.h.inc -gen-op-decls)
mlir_tablegen(EDAOps.cpp.inc -gen-op-defs)
add_public_tablegen_target(MLIREDAEOpsIncGen)

# 生成 Types
set(LLVM_TARGET_DEFINITIONS EDATypes.td)
mlir_tablegen(EDATypes.h.inc -gen-typedef-decls)
mlir_tablegen(EDATypes.cpp.inc -gen-typedef-defs)
add_public_tablegen_target(MLIREDAETypesIncGen)

# 设置所有 TableGen 目标
set(MLIR_EDA_TABLEGEN_TARGETS
  MLIREDAEDialectIncGen
  MLIREDAEOpsIncGen
  MLIREDAETypesIncGen
)