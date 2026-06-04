cmake_minimum_required(VERSION 3.16)
project(rk3588_video_ai LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# OpenCV 先用当前系统可用版本。
# 如果板端没有安装系统 OpenCV，再把库放到 third_party/opencv。
set(PROJECT_OPENCV_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/opencv/lib/cmake/opencv4")

if(EXISTS "${PROJECT_OPENCV_DIR}/OpenCVConfig.cmake")
    set(OpenCV_DIR "${PROJECT_OPENCV_DIR}")
endif()

find_package(OpenCV REQUIRED COMPONENTS core imgcodecs videoio)

add_executable(rk_video_ai
    src/app/main.cpp
    src/app/cli_args.cpp
    src/config/app_config.cpp
    src/config/stream_config.cpp
    src/logging/logger.cpp
    src/video/buffer/frame_buffer.cpp
)

target_include_directories(rk_video_ai PRIVATE include)
target_link_libraries(rk_video_ai PRIVATE ${OpenCV_LIBS})
