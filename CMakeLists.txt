cmake_minimum_required(VERSION 3.15)
project(benchypp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(example examples/example.cpp)
target_include_directories(example PRIVATE include)
