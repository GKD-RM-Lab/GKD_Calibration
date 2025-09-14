# GKD 相机标定工具

基于海康威视相机和OpenCV的相机标定工具。

## 概述

本项目提供了一个相机标定工具，使用棋盘格图案对标定海康威视相机。它从相机捕获图像，检测棋盘格角点，并计算相机的内参和畸变系数。

## 功能特性

基于棋盘格图案的相机标定
海康威视相机支持
基于OpenCV的图像处理
自动角点检测
内参计算（相机矩阵和畸变系数）
可配置的标定参数

## 依赖项

- OpenCV
- 海康威视SDK (MVS)
- OpenVINO
- X11
- nlohmann_json
- CMake (>= 3.16)
- 支持C++17的编译器

## 构建项目

### 先决条件

1. 安装OpenCV开发库
2. 安装OpenVINO工具包
3. 安装nlohmann_json库

### 构建说明

```bash
# 给构建脚本添加执行权限
chmod +x build.sh

# 运行构建脚本
./build.sh
```

这将在`build`目录中生成可执行文件。

或者，您也可以手动构建：

```bash
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DHIK_SDK_PATH=/opt/MVS
make -j$(nproc)
```

## 使用方法

### 运行标定

构建完成后，您可以运行标定工具：

```bash
# 给运行脚本添加执行权限
sudo ./run.sh
```
### 配置参数

标定参数可以在 `configs/gkd_calibrate_configs.yaml` 中调整：

- `boardSize_h`: 棋盘格高度（内角点数量）
- `boardSize_w`: 棋盘格宽度（内角点数量）
- `squareSize`: 棋盘格每个方格的大小（单位：mm）
- `img_count`: 用于标定的图像数量
- `sample_period`: 图像采样周期（帧数）
- `calib_yaml_path`: 相机内参输出路径
- `cam_gain`: 相机增益设置
- `cam_exptime`: 相机曝光时间
- `framerate`: 相机帧率

## 项目结构

```
gkd_calibration/
├── calibration/           # 标定源代码
│   ├── cali_HIKdriver.*  # 海康威视相机驱动
│   ├── calibrate_gkd.*   # 主要标定实现
│   └── calibrate_parameter_gkd.*  # 标定参数
├── configs/              # 配置文件
├── build.sh              # 构建脚本
├── run.sh                # 运行脚本
├── CMakeLists.txt        # CMake构建配置
└── README.md             # 本文件
```

## 标定流程

1. 准备已知方格尺寸的棋盘格图案
2. 将海康威视相机连接到系统
3. 使用`--calibration`参数运行标定工具
4. 从不同角度向相机展示棋盘格图案
5. 系统将自动捕获图像并检测角点
6. 收集足够图像后，将计算标定参数
7. 结果将保存到指定的YAML文件中

## 输出结果

标定结果以YAML文件格式保存，包含：
- 相机矩阵（内参）
- 畸变系数
- 每个标定图像的旋转向量和平移向量

## 故障排除

1. **相机未检测到**：确保海康威视相机已正确连接，且MVS SDK已正确安装
2. **棋盘格未检测到**：确保光照充足，棋盘格图案清晰可见
3. **构建错误**：验证所有依赖项已安装，且CMakeLists.txt中的路径正确
