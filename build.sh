#!/bin/bash

# 创建构建目录
mkdir -p build
cd build

# 配置CMake (可以根据需要调整HIK SDK路径)
cmake .. -DCMAKE_BUILD_TYPE=Release -DHIK_SDK_PATH=/opt/MVS

# 编译项目
make -j$(nproc)

echo "Build completed. Executable: ./gkd_calibration"
echo "Usage: ./gkd_calibration --calibration"
