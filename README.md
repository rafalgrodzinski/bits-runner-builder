# Bits Runner Builder
Welcome to Bits Runner Builder! Compiler for the Bits Runner Code (BRC) language 🤘

## Quick links
- [BRC Language Reference](docs/Reference.md)
- [Casting Rules](docs/Casts.md)
- [Extra Information](docs/Extra.md)

## Overview
Bits Runner Builder is a compiler for the Bits Runner Code language, which has been designed for the [Bits Runner](https://github.com/rafalgrodzinski/bits-runner) operating system. It aims to be an opinionated, low-level language, a modernised C with revised syntax and a number of quality of life improvement. The functinality is simple, transparent, and explicit. Altough it has a simple class-like functionality, class hierarchies, templates, or other unnecessary fluff is not supported.

It has been been built with LLVM so it should be fairly performant. Keep in mind that it is still work in progress so not everything is finished and there is still probably plenty of bugs and gremlins hiding around 🐝 If you find any, let me know, additional pair of eyes is always helpful.

## Main features
BRC allows for low-level system programming, so one of the main features is a seamless support for inline assembly, pointers, and explicit data handling. For this reason types have explicit sizes, there is no runtime and the memory is manually managed.

The highlights of the languge are:
- Modules without headers
- Pointers use instance properties instead of operators
- Casting also uses member syntax
- No curly braces for scope or semicollons
- Functions inside of `blob` types for simple class-like funtionality
- `if-else` statements are expressions
- Explicit type sizes (integers, floats, etc)
- `for`, `while`, `do-while` loops integrated into single `rep` keyword
- Directly supports decimal, hex, binary numbers with `_` separator between digits
- Shows tokens, AST, and build statistics for each phase `--verb=v2` or `v3`

## Examples
```
// Basic hello world
@module main

@extern putchar fun: character u8 -> u32

@export main fun -> u32
    text data<u8> <- "Hello, world!\n"
    
    rep i u32 <- 0, text[i] != 0, i <- i + 1: putchar(text[i])

    ret 0
;
```

## But why?
The idea was to build the whole computing environment from scratch which can be its own thing. Many project of this kind try to be sort of recoding of C/Unix, but this is not the point here. This project doesn't aim at compatibility with existing sfotware so it may hapilly break things in order to make things simpler, more modern, or just different.

It's mostly a learning opportunity and a bit of fun, but maybe you can find some bits of interesting knowledge for your own project.

## Quick Start
To try out BRC download an appropriate build from the available releases and make sure that you have LLVM 20 installed on your system.

Required dependencies
- clang 17.0.0 and up
- LLVM 20.1.0 until 21
- CMake 4.0.0 and up

### macOS
Make sure you have [Homebrew](https://brew.sh/) installed
- LLVM: `brew install llvm@20`
- CMake: `brew install cmake`

### Linux
Tested on Ubuntu, but should be similar for other distributions
- LLVM: `sudo apt install llvm-20`
- CMake: Version 4 wasn't available, so it had to be installed manually from [cmake.org](https://cmake.org/download/)

### Windows
Tested on Windows 11
- LLVM: From [LLVM Github releases](https://github.com/llvm/llvm-project/releases) or you can just got directly for this version [LLVM-20.1.8-win64.exe](https://github.com/llvm/llvm-project/releases/download/llvmorg-20.1.8/LLVM-20.1.8-win64.exe)
- Cmake: As for Linux, download directly version at least 4.0.0 [cmake.org](https://cmake.org/download/)


### Building
```
cmake -B build
cmake --build build --config Release
// or
cmake --build build --config Debug
```
You'll then be able to find the executable under `build/brb`.

There are "Build (Debug)" and "Clean" tasks specified for VSCode. There is also a launch configuartion, which you can launch by pressing F5 which will then build and start debugging using command `brb --verb=v3 samples/test.brc`. You'll need to have "LLDB DAP" extension installed in VSCode and `lldb-dap` on your system.

## Samples
Inside of `samples/` there is a bunch of different sample code using different features of the code. Inside each of them there is a `build.sh`, which will generate an executable. You can also Run `samples/run_all.sh` to build and run each of the samples, which is useful for testing to check if everything works as expected.

`callback`:
Uses function pointers to implement basic callback functionality.

`casts`:
Demonstrates how to use casting for for simple and data types.

`external_linkage`:
Builds two separate object files and then links them together.

`fib`:
Fibonaci numbers, demonstrates recursion.

`fizz_buzz`:
Fizz buzz is useful for easily figuring out if someone know anythign about programming, but here is uses multi-level if-else expressions with more complex conditions.

`hello`:
No programming language is complete without a hello world introduction.

`linked_list`:
Uses pointers and memory allocation for implementing a single linked list.

`merge_sort`:
Implementation of merge sort to show usage of data types.

`multi_module`:
Shows how code can be split into separat files and modules.

`primes`:
Based on ["Software Drag Racing!" by Dave Plummer](https://github.com/PlummersSoftwareLLC/Primes/tree/drag-race). Runs as many iterrations as possible of calculating primes up to 1,000,000 in 5 seconds and reports the number or iterrations. I got a bit under 1200 on my Intel Core i7 14700.

`strings`:
This uses the basic library `@B` to manipulate strings.

## Additional Stuff
BRC language support for Visual Studio Code: [https://github.com/rafalgrodzinski/brc-vscode](https://github.com/rafalgrodzinski/brc-vscode).
