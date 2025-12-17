# EDA Dialect 单元测试配置

# 找到所有测试目录
file(GLOB TEST_SUBDIRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*")
list(FILTER TEST_SUBDIRS INCLUDE REGEX "^[A-Za-z].*")

# 为每个测试目录创建测试
foreach(subdir ${TEST_SUBDIRS})
  if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${subdir}")
    # 收集该目录下的所有 .cpp 文件
    file(GLOB_RECURSE TEST_SOURCES
      "${CMAKE_CURRENT_SOURCE_DIR}/${subdir}/*.cpp"
    )

    # 如果有测试源文件
    if(TEST_SOURCES)
      # 创建测试可执行文件
      string(REPLACE "/" "" test_target_name "EDA${subdir}Tests")
      add_executable(${test_target_name} ${TEST_SOURCES})

      # 添加包含目录
      target_include_directories(${test_target_name} PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_BINARY_DIR}/include
        ${MLIR_INCLUDE_DIRS}
      )

      # 链接库
      target_link_libraries(${test_target_name}
        PRIVATE
        GTest::gtest
        GTest::gtest_main
        MLIRIR
        MLIRSupport
        MLIRPass
        MLIRFuncDialect
        MLIRArithDialect
        MLIREDA
        EDATransformation
      )

      # 添加 TableGen 依赖
      if(TARGET MLIREDAEDialectIncGen)
        add_dependencies(${test_target_name} MLIREDAEDialectIncGen)
      endif()
      if(TARGET MLIREDAEOpsIncGen)
        add_dependencies(${test_target_name} MLIREDAEOpsIncGen)
      endif()
      if(TARGET MLIREDAETypesIncGen)
        add_dependencies(${test_target_name} MLIREDAETypesIncGen)
      endif()

      # 添加测试
      add_test(NAME ${test_target_name} COMMAND ${test_target_name})

      # 设置测试属性
      set_tests_properties(${test_target_name} PROPERTIES
        TIMEOUT 60
        LABELS "EDADialect"
      )
    endif()
  endif()
endforeach()