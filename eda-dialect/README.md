# EDA Dialect for MLIR

这个项目实现了一个专门用于 EDA (Electronic Design Automation) 工具开发的 MLIR Dialect，支持类型安全的 TCL 代码生成。

## 概述

EDA Dialect 提供了以下功能：
- 类型安全的变量操作
- 算术和逻辑运算
- 控制流结构（if、for、while）
- 函数定义和调用
- 文件系统操作
- 优化的代码生成

## 项目结构

```
eda-dialect/
├── CMakeLists.txt           # 主构建文件
├── README.md                # 项目说明
├── include/eda-dialect/     # 头文件
│   ├── Dialect.h           # Dialect 定义
│   ├── Ops.h               # 操作声明
│   ├── Types.h             # 类型定义
│   ├── EDADialect.td       # TableGen Dialect 定义
│   ├── EDAOps.td           # TableGen 操作定义
│   └── EDATypes.td         # TableGen 类型定义
├── lib/
│   ├── Dialect/            # Dialect 实现
│   │   ├── Dialect.cpp
│   │   ├── Ops.cpp
│   │   └── Types.cpp
│   └── Transformation/     # 优化 Pass
│       ├── ConstantFold.cpp
│       ├── DeadCodeElimination.cpp
│       └── LoopUnroll.cpp
├── tools/                  # 命令行工具
│   ├── mlir-eda-opt.cpp    # 优化工具
│   └── mlir-eda-translate.cpp # 代码生成工具
├── examples/               # 示例代码
│   ├── basic_ops.mlir
│   ├── control_flow.mlir
│   └── optimization.mlir
└── unittest/               # 单元测试
    ├── CMakeLists.txt
    ├── Dialect
    ├── Transformation
    └── Translation
```

## 构建说明

### 前提条件
- LLVM/MLIR 18.0 或更高版本
- CMake 3.15.4 或更高版本
- C++17 兼容的编译器

### 构建步骤

1. 构建并安装 LLVM/MLIR：
```bash
cd llvm-project
mkdir build && cd build
cmake -G Ninja ../llvm \
  -DCMAKE_INSTALL_PREFIX=<install-dir> \
  -DLLVM_ENABLE_PROJECTS=mlir \
  -DLLVM_TARGETS_TO_BUILD="Native" \
  -DCMAKE_BUILD_TYPE=Release
ninja install
```

2. 构建 EDA Dialect：
```bash
cd eda-dialect
mkdir build && cd build
cmake .. -G Ninja \
  -DMLIR_DIR=<install-dir>/lib/cmake/mlir \
  -DLLVM_DIR=<install-dir>/lib/cmake/llvm \
  -DCMAKE_BUILD_TYPE=Release
ninja
```

## 使用示例

### 基本操作
```mlir
module {
  %timeout = "eda.assign"("timeout", 10) : i32
  %debug = "eda.assign"("debug", true) : i1

  %sum = "eda.add"(%timeout, 5) : i32
  "eda.puts"("Timeout sum: %d", %sum) : ()
}
```

### 控制流
```mlir
module {
  %timeout_val = "eda.load"("timeout") : i32
  %cmp = "eda.cmp_gt"(%timeout_val, 15) : i1

  "eda.if"(%cmp) ({
    "eda.puts"("Timeout is too high!") : ()
  }) : ()
}
```

## 工具使用

### mlir-eda-opt
用于优化 MLIR 代码：
```bash
./mlir-eda-opt example.mlir -constant-fold -dead-code-elimination -o optimized.mlir
```

### mlir-eda-translate
用于将 MLIR 转换为 TCL：
```bash
./mlir-eda-translate example.mlir -mlir-to-tcl > output.tcl
```

## 开发计划

- [x] 基础 Dialect 结构
- [ ] 核心操作实现
- [ ] 类型系统完善
- [ ] 优化 Pass 开发
- [ ] 代码生成器
- [ ] 测试用例

## 贡献指南

欢迎提交 Issue 和 Pull Request。在提交代码前，请确保：
1. 代码符合项目风格
2. 添加适当的测试
3. 更新相关文档

## 许可证

本项目采用 Apache 2.0 许可证。