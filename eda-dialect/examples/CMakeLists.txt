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
