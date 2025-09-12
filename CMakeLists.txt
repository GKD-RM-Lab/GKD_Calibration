cmake_minimum_required(VERSION 3.16)
project(gkd_calibration)

# ==========================
# 基础设置
# ==========================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# ==========================
# 查找依赖包
# ==========================
find_package(OpenCV REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED)
find_package(OpenVINO REQUIRED)
find_package(X11 REQUIRED)
find_package(nlohmann_json REQUIRED)

# ==========================
# 包含目录
# ==========================
include_directories(
    ${OpenCV_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/calibration/include
    ${CMAKE_CURRENT_SOURCE_DIR}/calibration/lib/include
    ${CMAKE_CURRENT_SOURCE_DIR}/calibration
)

# ==========================
# 海康 SDK 配置
# ==========================
set(HIK_SDK_PATH "${CMAKE_CURRENT_SOURCE_DIR}/calibration/lib")

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(HIK_LIB_DIR "${HIK_SDK_PATH}/64")
else()
    set(HIK_LIB_DIR "${HIK_SDK_PATH}/32")
endif()

if(EXISTS ${HIK_LIB_DIR})
    # 查找海康威视相关库
    find_library(HIK_CAMERA_LIB
        NAMES MvCameraControl
        PATHS ${HIK_LIB_DIR}
        NO_DEFAULT_PATH
    )
    
    find_library(HIK_GIGE_LIB
        NAMES MVGigEVisionSDK
        PATHS ${HIK_LIB_DIR}
        NO_DEFAULT_PATH
    )
    
    find_library(HIK_USB3_LIB
        NAMES MvUsb3vTL
        PATHS ${HIK_LIB_DIR}
        NO_DEFAULT_PATH
    )

    if(NOT HIK_CAMERA_LIB)
        message(FATAL_ERROR "Cannot find MvCameraControl library in ${HIK_LIB_DIR}")
    else()
        message(STATUS "Found HIK SDK library: ${HIK_CAMERA_LIB}")
        if(HIK_GIGE_LIB)
            message(STATUS "Found HIK GigE library: ${HIK_GIGE_LIB}")
        endif()
        if(HIK_USB3_LIB)
            message(STATUS "Found HIK USB3 library: ${HIK_USB3_LIB}")
        endif()
    endif()
    
else()
    message(FATAL_ERROR "HIK SDK directory not found at ${HIK_LIB_DIR}")
endif()

# ==========================
# 源文件与头文件
# ==========================
set(SOURCES
    calibration/calibrate_gkd_main.cpp
    calibration/calibrate_gkd.cpp
    calibration/calibrate_parameter_gkd.cpp
    calibration/cali_HIKdriver.cpp
)

set(HEADERS
    calibration/calibrate_gkd.hpp
    calibration/calibrate_parameter_gkd.hpp
    calibration/cali_HIKdriver.hpp
    calibration/lib/include/CameraParams.h
    calibration/lib/include/MvCameraControl.h
    calibration/lib/include/MvErrorDefine.h
    calibration/lib/include/PixelType.h
)

# ==========================
# 可执行文件
# ==========================
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# 设置运行时库路径
set_target_properties(${PROJECT_NAME} PROPERTIES
    BUILD_RPATH "${HIK_LIB_DIR}"
    INSTALL_RPATH "${HIK_LIB_DIR}"
)

# ==========================
# 链接库
# ==========================
# 准备海康威视库列表
set(HIK_LIBRARIES ${HIK_CAMERA_LIB})
if(HIK_GIGE_LIB)
    list(APPEND HIK_LIBRARIES ${HIK_GIGE_LIB})
endif()
if(HIK_USB3_LIB)
    list(APPEND HIK_LIBRARIES ${HIK_USB3_LIB})
endif()

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        ${OpenCV_LIBS}
        ${HIK_LIBRARIES}
        openvino::runtime
        Threads::Threads
        ${X11_LIBRARIES}
        pthread
        nlohmann_json::nlohmann_json
        dl   # 动态库依赖
        rt   # 可能需要的实时库
)

# ==========================
# 安装规则
# ==========================
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)

install(DIRECTORY configs/
    DESTINATION share/${PROJECT_NAME}/configs
    FILES_MATCHING PATTERN "*.yaml"
)

# ==========================
# 输出信息
# ==========================
message(STATUS "OpenCV version: ${OpenCV_VERSION}")
message(STATUS "OpenCV include dirs: ${OpenCV_INCLUDE_DIRS}")
message(STATUS "OpenCV libraries: ${OpenCV_LIBS}")
message(STATUS "HIK SDK path: ${HIK_LIB_DIR}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
