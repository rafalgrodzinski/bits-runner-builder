# Bits Runner Builder
Welcome to Bits Runner Builder!

## Quick links
- [Language Reference](Reference.md)
- [Detailed Syntax](Syntax.md)

## Overview
Bits Runner Builder (brb) is a compiler for Bits Runner Code (brc) language, which has been designed for the Bits Runner operating system. It aims to be an opinionated, low-level language, a sort of improved C while providing a revised syntax and a couple of quality of life improvement. It's a simple system programming language, so no class hierarchies, templates, or other unnecessary fluff.

It has been been built with LLVM so it should be fairly performant.

## Main features
BRC allows for low-level system programming, so one of the main features is a seamless support for embeded assembly, pointers mainipulation and explicit data handling. For this reason data types have explicit byte-sizes, there is no runtime and the memory is manually managed.

The language aims to be simple, easy to reason about and predictable. For this reason there a class-like features, but no inheritance. Composition is much better anyway and doesn't lead to incomprehensible codebases (did I mention that it's opinionated?).

## Examples
```
// Basic hello world
//
@extern putchar fun: character u32 -> u32

main fun -> u32
    text data<u8> <- "Hello, world!\n"
    
    rep i u32 <- 0, text[i] != 0:
        putchar(text[i])
        i <- i + 1
    ;

    ret 0
;
```

## But why?
The idea was to build the whole computing environment from scratch which can be its own thing. Many project of this kind try to be sort of recoding of C/Unix, but this is not the point in the case. This project doesn't aim at compatibility so it may hapilly break things in order to make things simpler, more modern, or just different.

It's mostly a learning opportunity and a bit of fun, but maybe you can find some bits of interesting knowledge for your own project.

## Samples
[Hello World](samples/hello.brc)

[Fibonaci Numbers](samples/fib.brc)

#### How to build the samples
```
brb samples/hello.brc
cc -o hello hello.o
```