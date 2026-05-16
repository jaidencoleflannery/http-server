cmake_minimum_required(VERSION 3.20)
project(http-server C)

add_executable(http-server main.c)

target_include_directories(http-server PRIVATE include/)
target_compile_options(http-server PRIVATE -Wall -Wextra -std=c17)

set(CMAKE_BUILD_TYPE Debug)

