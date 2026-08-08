cmake_minimum_required(VERSION 3.20)

project(melcpp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(melcpp
    src/main.cpp
    src/loop.cpp
)

target_include_directories(melcpp
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_options(melcpp
    PRIVATE
        -Wall -Wextra -Wpedantic
)
