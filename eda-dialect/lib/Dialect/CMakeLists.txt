# EDA Dialect 库构建配置

# 设置源文件
set(EDA_DIALECT_SOURCES
  Dialect.cpp
  Ops.cpp
  Types.cpp
)

# 创建 EDA Dialect 库
add_mlir_dialect_library(MLIREDA
  ${EDA_DIALECT_SOURCES}

  ADDITIONAL_HEADER_DIRS
  ${PROJECT_SOURCE_DIR}/include/eda-dialect

  DEPENDS
  MLIREDAEDialectIncGen
  MLIREDAEOpsIncGen
  MLIREDAETypesIncGen

  LINK_LIBS PUBLIC
  MLIRIR
  MLIRInferTypeOpInterface
  MLIRFuncDialect
  MLIRArithDialect
  MLIRSCFDialect
)