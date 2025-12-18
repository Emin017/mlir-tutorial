# EDA Dialect 示例配置

add_executable(test_translate
  test_translate.cpp
)

target_link_libraries(test_translate
  PRIVATE
  MLIRIR
  MLIRSupport
  MLIREDA
  EDATransformation
)

install(TARGETS test_translate
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Test TCL code generation
add_executable(test_tcl_generation
  test_tcl_generation.cpp
)

target_link_libraries(test_tcl_generation
  PRIVATE
  MLIRIR
  MLIRSupport
  MLIRFuncDialect
  MLIREDA
  EDATransformation
)

install(TARGETS test_tcl_generation
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Test advanced TCL code generation
add_executable(test_tcl_advanced
  test_tcl_advanced.cpp
)

target_link_libraries(test_tcl_advanced
  PRIVATE
  MLIRIR
  MLIRSupport
  MLIRFuncDialect
  MLIREDA
  EDATransformation
)

install(TARGETS test_tcl_advanced
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
