cmake_minimum_required(VERSION 3.16)
project(rk3588_video_ai LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(rk_video_ai
    src/app/main.cpp
    src/logging/logger.cpp
)

target_include_directories(rk_video_ai PRIVATE include)
