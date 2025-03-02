# mlir-tutorial

Source code for the MLIR tutorial at [KEKE046/mlir-tutorial](https://github.com/KEKE046/mlir-tutorial).

## Compile
First, we need to compile the MLIR:
```shell
cd llvm-project
mkdir build && cd build
cmake -G Ninja ../llvm \
  -DCMAKE_INSTALL_PREFIX=/mlir-tutorial/install \
  -DLLVM_ENABLE_PROJECTS=mlir \
  -DLLVM_BUILD_EXAMPLES=ON \
  -DLLVM_TARGETS_TO_BUILD="Native;NVPTX;AMDGPU" \
  -DCMAKE_BUILD_TYPE=Release \
  -DLLVM_ENABLE_ASSERTIONS=ON -DLLVM_PARALLEL_LINK_JOBS=2 -DLLVM_RAM_PER_LINK_JOB=12000

ninja
```

Then, we can compile the mlir-toy:
```shell
cd mlir-toy
mkdir build && cd build
cmake .. -GNinja -DCMAKE_INSTALL_PREFIX=/mlir-tutorial/install
ninja
```