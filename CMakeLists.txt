cmake_minimum_required(VERSION 4.0)
project(bits-runner-builder LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

find_package(LLVM REQUIRED CONFIG)
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})

file(GLOB_RECURSE SOURCES
    src/*.cpp
    src/**/*.cpp)
add_executable(brb ${SOURCES})

target_include_directories(brb PRIVATE ${CMAKE_SOURCE_DIR}/src)

target_link_libraries(brb llvm)
