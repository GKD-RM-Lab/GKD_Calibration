cmake_minimum_required(VERSION 3.16)
project(gkd_calibration)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置编译类型
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# 编译选项
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# 查找依赖包
find_package(OpenCV REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED)

# 查找OpenVINO
find_package(OpenVINO REQUIRED)

# 查找X11库 (Linux GUI支持)
find_package(X11 REQUIRED)

# 包含目录
include_directories(
    ${OpenCV_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/calibration/include
    ${CMAKE_CURRENT_SOURCE_DIR}/calibration
)

# 海康威视SDK库路径 (需要根据实际安装路径调整)
set(HIK_SDK_PATH "/opt/MVS" CACHE PATH "HIK SDK installation path")
if(EXISTS ${HIK_SDK_PATH})
    include_directories(${HIK_SDK_PATH}/include)
    link_directories(${HIK_SDK_PATH}/lib/64)
    set(HIK_LIBRARIES MvCameraControl)
else()
    message(WARNING "HIK SDK not found at ${HIK_SDK_PATH}. Please set HIK_SDK_PATH to correct path.")
    set(HIK_LIBRARIES "")
endif()

# 源文件
set(SOURCES
    calibration/calibrate_gkd_main.cpp
    calibration/calibrate_gkd.cpp
    calibration/calibrate_parameter_gkd.cpp
    calibration/cali_HIKdriver.cpp
)

# 头文件
set(HEADERS
    calibration/calibrate_gkd.hpp
    calibration/calibrate_parameter_gkd.hpp
    calibration/cali_HIKdriver.hpp
    calibration/include/CameraParams.h
    calibration/include/MvCameraControl.h
    calibration/include/MvErrorDefine.h
    calibration/include/PixelType.h
)

# 创建可执行文件
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# 链接库
target_link_libraries(${PROJECT_NAME}
    ${OpenCV_LIBS}
    ${HIK_LIBRARIES}
    openvino::runtime
    Threads::Threads
    ${X11_LIBRARIES}
    pthread
)

# 安装规则
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)

# 安装配置文件
install(DIRECTORY configs/
    DESTINATION share/${PROJECT_NAME}/configs
    FILES_MATCHING PATTERN "*.yaml"
)

# 打印信息
message(STATUS "OpenCV version: ${OpenCV_VERSION}")
message(STATUS "OpenCV include dirs: ${OpenCV_INCLUDE_DIRS}")
message(STATUS "OpenCV libraries: ${OpenCV_LIBS}")
message(STATUS "HIK SDK path: ${HIK_SDK_PATH}")

# 编译选项说明
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
