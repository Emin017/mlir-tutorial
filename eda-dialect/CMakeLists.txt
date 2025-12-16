# EDA Dialect CMakeLists.txt

cmake_minimum_required(VERSION 3.15.4)
project(EDA_Dialect)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find MLIR
find_package(MLIR REQUIRED CONFIG)
message(STATUS "Found MLIR: ${MLIR_DIR}")
message(STATUS "Found LLVM: ${LLVM_DIR}")

# Add MLIR CMake extensions
list(APPEND CMAKE_MODULE_PATH "${MLIR_CMAKE_DIR}")
include(AddMLIR)
include(TableGen)

# Set source files
set(EDA_DIALECT_SOURCES
  lib/Dialect/Dialect.cpp
  lib/Dialect/Ops.cpp
  lib/Dialect/Types.cpp
)

set(EDA_DIALECT_HEADERS
  include/eda-dialect/Dialect.h
  include/eda-dialect/Ops.h
  include/eda-dialect/Types.h
)

# Add TableGen files for operations
set(EDA_DIALECT_TD_FILES
  include/eda-dialect/EDADialect.td
  include/eda-dialect/EDAOps.td
  include/eda-dialect/EDATypes.td
)

# Generate op documentation
mlir_tablegen(EDAOps.inc.inc -gen-op-defs ${EDA_DIALECT_TD_FILES})
mlir_tablegen(EDAOps.h.inc -gen-op-decls ${EDA_DIALECT_TD_FILES})
mlir_tablegen(EDADialect.h.inc -gen-dialect-decls ${EDA_DIALECT_TD_FILES})
mlir_tablegen(EDATypes.h.inc -gen-typedef-decls ${EDA_DIALECT_TD_FILES})
mlir_tablegen(EDATypes.inc.inc -gen-typedef-defs ${EDA_DIALECT_TD_FILES})

# Add public headers
set(public_headers ${EDA_DIALECT_HEADERS})
foreach(header ${public_headers})
  set_property(GLOBAL PROPERTY EDA_DIALECT_PUBLIC_HEADERS ${public_headers})
endforeach()

# Create EDA Dialect library
add_mlir_library(EDADialect
  ${EDA_DIALECT_SOURCES}

  EXCLUDE_FROM_LIBMLIR
  ADDITIONAL_HEADER_DIRS
  ${PROJECT_SOURCE_DIR}/include/eda-dialect

  DEPENDS
  EDAOpsIncGen
  EDATypesIncGen
  EDADialectIncGen

  LINK_COMPONENTS
  Core
  Support

  LINK_LIBS
  MLIRIR
  MLIRSupport
  MLIROps
  MLIRPass
)

# Add optimization passes
file(GLOB_RECURSE TRANSFORMATION_SOURCES "lib/Transformation/*.cpp")
add_mlir_library(EDATransformation
  ${TRANSFORMATION_SOURCES}

  EXCLUDE_FROM_LIBMLIR

  LINK_COMPONENTS
  Core
  Support

  LINK_LIBS
  MLIRIR
  MLIRSupport
  MLIROps
  MLIRPass
  EDADialect
)

# Add mlir-eda-opt tool
add_mlir_tool(mlir-eda-opt
  tools/mlir-eda-opt.cpp

  DEPENDS
  EDAOpsIncGen

  LINK_LIBS
  MLIRIR
  MLIRSupport
  MLIROps
  MLIRPass
  MLIRTransformUtils
  EDADialect
  EDATransformation
)

# Add mlir-eda-translate tool
add_mlir_tool(mlir-eda-translate
  tools/mlir-eda-translate.cpp

  DEPENDS
  EDAOpsIncGen

  LINK_LIBS
  MLIRIR
  MLIRSupport
  MLIROps
  MLIRPass
  MLIRTransformUtils
  EDADialect
  EDATransformation
)

# Install
install(TARGETS EDADialect EDATransformation
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

install(FILES ${public_headers}
  DESTINATION include/eda-dialect
)

install(TARGETS mlir-eda-opt mlir-eda-translate
  RUNTIME DESTINATION bin
)

# Test
add_subdirectory(unittest)