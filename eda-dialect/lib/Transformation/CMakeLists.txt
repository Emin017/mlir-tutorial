# EDA Dialect Transformation 库构建配置

# 收集所有转换源文件
file(GLOB_RECURSE EDA_TRANSFORMATION_SOURCES
  "*.cpp"
)

# 如果有 Pass 文件，添加其生成的目标
set(TRANSFORM_DEPENDS)
if(EXISTS "${PROJECT_SOURCE_DIR}/include/eda-dialect/EDAPasses.td")
  list(APPEND TRANSFORM_DEPENDS MLIREDAEPassesIncGen)
endif()

# 创建 Transformation 库
add_mlir_library(EDATransformation
  ${EDA_TRANSFORMATION_SOURCES}

  EXCLUDE_FROM_LIBMLIR

  DEPENDS
  ${TRANSFORM_DEPENDS}

  LINK_COMPONENTS
  Core
  Support

  LINK_LIBS
  MLIRIR
  MLIRSupport
  MLIRPass
  MLIRTransformUtils
  MLIREDA
)