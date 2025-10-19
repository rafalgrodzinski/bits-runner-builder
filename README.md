# Bits Runner Builder
Welcome to Bits Runner Builder! Compiler for the Bits Runner Code (BRC) language 🤘

## Quick links
- [BRC Language Reference](docs/Reference.md)
- [Extra Information](docs/Extra.md)

## Overview
Bits Runner Builder is a compiler for Bits Runner Code (brc) language, which has been designed for the [Bits Runner](https://github.com/rafalgrodzinski/bits-runner) operating system. It aims to be an opinionated, low-level language, a sort of improved C while providing a revised syntax and a couple of quality of life improvement. It's a simple system programming language, so no class hierarchies, templates, or other unnecessary fluff.

It has been been built with LLVM so it should be fairly performant. Keep in mind that it is still work in progress so not everything is finished and there is still probably plenty of bugs and gremlins hiding around 🙈

## Main features
BRC allows for low-level system programming, so one of the main features is a seamless support for embeded assembly, pointers mainipulation, and explicit data handling. For this reason data types have explicit byte-sizes, there is no runtime and the memory is manually managed.

The language aims to be simple, easy to reason about, and predictable. Because of this there a class-like features, but no inheritance. Composition is much better anyway and doesn't lead to incomprehensible codebases (did I mention that it's opinionated?).

## Examples
```
// Basic hello world
//
@module main

@extern putchar fun: character u64 -> u32

@export main fun -> u64
    text data<u64> <- "Hello, world!\n"
    
    rep i u64 <- 0, text[i] != 0, i <- i + 1
        putchar(text[i])
    ;

    ret 0
;
```

## But why?
The idea was to build the whole computing environment from scratch which can be its own thing. Many project of this kind try to be sort of recoding of C/Unix, but this is not the point in the case. This project doesn't aim at compatibility so it may hapilly break things in order to make things simpler, more modern, or just different.

It's mostly a learning opportunity and a bit of fun, but maybe you can find some bits of interesting knowledge for your own project.

## Quick Start
Make sure that you have cmake, llvm, and lld installed on your system.
```
cmake -B build
cmake --build build --config Release
// or
cmake --build build --config Debug
```
You'll then be able to finde the executable under `build/brb`.

There are also "Build (Debug)" and "Clean" tasks specified for VSCode. There is also a launch configuartion, which you can launch by pressing F5 will will then build and start debugging using command `brb -v samples/test.brc`. You'll need to have "LLDB DAP" extension installed in VSCode.

## Samples
Hello World
[samples/hello/](samples/hello/)

Fibonaci Numbers
[samples/fib/](samples/fib/)

Merge Sort
[samples/merge_sort/](samples/merge_sort/)

Linked List
[samples/linked_list/](samples/linked_list/)

Fizz Buzz
[samples/fizz_buzz/](samples/fizz_buzz/)

Casts
[samples/casts/](samples/casts/)

#### How to build & run the samples

Execute from the main directory
```
// Hello World
./samples/hello/build.sh
./hello

// Fibonaci Numbers
./samples/fib/build.sh
./fib

// Merge Sort
./samples/merge_sort/build.sh
./merge_sort

// Linked List
./samples/linked_list/build.sh
./linked_list

// Fizz Buzz
./samples/fizz_buzz/build.sh
./fizz_buzz

// Multi Module
./samples/multi_module/build.sh
./multi_module
```

## Additional Stuff
Language support for Visual Studio Code: [https://github.com/rafalgrodzinski/brc-vscode](https://github.com/rafalgrodzinski/brc-vscode).
