#!/bin/bash

# 创建构建目录
mkdir -p build
cd build

# 配置CMake (使用本地海康威视SDK)
cmake .. -DCMAKE_BUILD_TYPE=Release

# 编译项目
make -j$(nproc)

echo "Build completed. Executable: ./gkd_calibration"
echo "Usage: ./gkd_calibration --calibration"
echo ""
echo "Note: Make sure the following libraries are installed:"
echo "  - OpenCV (libopencv-dev)"
echo "  - OpenVINO"
echo "  - X11 development libraries (libx11-dev)"
echo "  - nlohmann-json (nlohmann-json3-dev)"
