add_mlir_tool(mlir-eda-translate
  mlir-eda-translate.cpp
)

target_link_libraries(mlir-eda-translate
  PRIVATE
  MLIRIR
  MLIRSupport
  MLIRParser
  MLIRPass
  MLIRTranslateLib
  MLIRFuncDialect
  MLIRArithDialect
  MLIREDA
  EDATransformation
)

install(TARGETS mlir-eda-translate
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
