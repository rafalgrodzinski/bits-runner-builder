cmake_minimum_required(VERSION 4.0)

file(READ "version.txt" VERSION)

project(
    bits-runner-builder
    VERSION ${VERSION}
    LANGUAGES CXX C
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_COLOR_DIAGNOSTICS OFF)
set(CMAKE_VERBOSE_MAKEFILE ON)

if(MSVC)
else()
	set(CMAKE_CXX_FLAGS "-Wno-parentheses")
endif()

set(VERSION_SUFFIX "$ENV{VERSION_SUFFIX}")
set(VERSION_SUFFIX "-dev-current" CACHE STRING "Version Suffix")

add_compile_definitions(VERSION=\"${VERSION}${VERSION_SUFFIX}\")

execute_process(
    COMMAND brew --prefix llvm@20
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

llvm_map_components_to_libnames(
	LLVM_LIBS

	core
	irreader
	passes
	mc
	support
	scalaropts
	target
	targetparser
	transformutils
	
	aarch64asmparser
	aarch64codegen
	
	amdgpuasmparser
	amdgpucodegen
	
	armasmparser
	armcodegen
	
	avrasmparser
	avrcodegen
	
	bpfasmparser
	bpfcodegen
	
	hexagonasmparser
	hexagoncodegen
	
	lanaiasmparser
	lanaicodegen
	
	loongarchasmparser
	loongarchcodegen
	
	mipsasmparser
	mipscodegen
	
	msp430asmparser
	msp430codegen
	
	nvptxcodegen
	
	powerpcasmparser
	powerpccodegen
	
	riscvasmparser
	riscvcodegen
	
	sparcasmparser
	sparccodegen
	
	spirvcodegen
	
	systemzasmparser
	systemzcodegen
	
	veasmparser
	vecodegen
	
	webassemblyasmparser
	webassemblycodegen
	
	x86codegen
	x86asmparser
	
	xcorecodegen 
)

target_link_libraries(brb ${LLVM_LIBS})
