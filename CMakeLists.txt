cmake_minimum_required(VERSION 3.14)
project(EffectiveMobileTest LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME} src/main.cpp src/bmp/Image.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE include)

