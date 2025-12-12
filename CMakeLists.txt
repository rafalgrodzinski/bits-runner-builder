cmake_minimum_required(VERSION 3.2)

file(READ "version.txt" VERSION)

project(
    bits-runner-builder
    VERSION ${VERSION}
    LANGUAGES CXX C
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_COLOR_DIAGNOSTICS OFF)
set(CMAKE_VERBOSE_MAKEFILE ON)

set(VERSION_SUFFIX "$ENV{VERSION_SUFFIX}")
set(VERSION_SUFFIX "-dev-current" CACHE STRING "Version Suffix")

add_compile_definitions(VERSION=\"${VERSION}${VERSION_SUFFIX}\")

execute_process(
    COMMAND brew --prefix llvm
    OUTPUT_VARIABLE LLVM_PATH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
set(ENV{PATH} "$ENV{PATH}:${LLVM_PATH}/bin")

find_package(LLVM 20.1.0...<21.0.0 REQUIRED CONFIG)
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})

file(GLOB_RECURSE SOURCES
    src/*.cpp
    src/**/*.cpp)
add_executable(brb ${SOURCES})

target_include_directories(brb PRIVATE ${CMAKE_SOURCE_DIR}/src)

target_link_libraries(brb LLVM)
