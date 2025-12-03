cmake_minimum_required(VERSION 4.0)

project(
    bits-runner-builder
    LANGUAGES CXX C
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_COLOR_DIAGNOSTICS OFF)
set(CMAKE_VERBOSE_MAKEFILE ON)

file(READ "version.txt" VERSION)
add_compile_definitions(VERSION=\"${VERSION}\")

execute_process(
    COMMAND brew --prefix llvm
    OUTPUT_VARIABLE LLVM_PATH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
set(ENV{PATH} "$ENV{PATH}:${LLVM_PATH}/bin")

find_package(LLVM REQUIRED CONFIG)
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})

file(GLOB_RECURSE SOURCES
    src/*.cpp
    src/**/*.cpp)
add_executable(brb ${SOURCES})

target_include_directories(brb PRIVATE ${CMAKE_SOURCE_DIR}/src)

target_link_libraries(brb llvm)
