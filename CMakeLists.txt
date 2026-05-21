set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

cmake_minimum_required(VERSION 3.20)
project(
    orb 
    VERSION 1.0
    DESCRIPTION "an efficient, scalable http server, built in c."
    LANGUAGES C
)

add_executable(
    orb
    ./src/http-server/main.c
)

target_compile_options(orb PRIVATE -Wall -Wextra -Wpedantic -std=c17)

target_include_directories(orb PRIVATE "src/shared/")

set(CMAKE_BUILD_TYPE Debug)

