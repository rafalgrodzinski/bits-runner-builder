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

@extern putchar fun: character u8 -> u32

@export main fun -> u64
    text data<u8> <- "Hello, world!\n"
    
    rep i u32 <- 0, text[i] != 0, i <- i + 1
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
Inside of `samples` there is a bunch of different samples using different features of the code. Inside each of them there is a `build.sh`, which will generate an executable. You can also Run `samples/run_all.sh` to build and run each of the samples, which is useful for testing if everything works as expected.

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

## Additional Stuff
Language support for Visual Studio Code: [https://github.com/rafalgrodzinski/brc-vscode](https://github.com/rafalgrodzinski/brc-vscode).
