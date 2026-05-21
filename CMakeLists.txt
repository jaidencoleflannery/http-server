set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

cmake_minimum_required(VERSION 3.20)
project(
    orb 
    VERSION 1.0
    DESCRIPTION "An efficient, scalable server, built in C."
    LANGUAGES C
)

add_library(
    shared
    ./src/shared/services/host-resolver/host_resolver.c
    ./src/shared/utilities/string-tools/string_tools.c 
)

add_executable(
    orb
    ./src/http-server/main.c
)

target_compile_options(orb PRIVATE -Wall -Wextra -Wpedantic -std=c17)

target_include_directories(orb PRIVATE "./src/shared/")
target_include_directories(shared PRIVATE "./src/shared/")
target_link_libraries(orb PRIVATE shared)

set(CMAKE_BUILD_TYPE Debug)

