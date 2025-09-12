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

# 海康威视SDK库路径
set(HIK_SDK_PATH "${CMAKE_CURRENT_SOURCE_DIR}/calibration/lib" CACHE PATH "HIK SDK installation path")

# 根据系统架构选择库目录
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    # 64位系统
    set(HIK_LIB_DIR "${HIK_SDK_PATH}/64")
else()
    # 32位系统
    set(HIK_LIB_DIR "${HIK_SDK_PATH}/32")
endif()

# 检查库目录是否存在
if(EXISTS ${HIK_LIB_DIR})
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/calibration/include)
    link_directories(${HIK_LIB_DIR})
    set(HIK_LIBRARIES MvCameraControl)
else()
    message(FATAL_ERROR "HIK SDK library directory not found at ${HIK_LIB_DIR}")
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
    calibration/lib/include/CameraParams.h
    calibration/lib/include/MvCameraControl.h
    calibration/lib/include/MvErrorDefine.h
    calibration/lib/include/PixelType.h
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

find_package(nlohmann_json REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)


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
