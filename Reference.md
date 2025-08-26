# BRC Language Reference

## Overview
Bits Runner Code (BRC) borrows a lot of concepts and syntax from C, but in a slightly modified way. The idea is to use familiar concept in a simplified way, avoiding usage of unnecessary fluff and just to make the code simpler and shorter, while avoiding any unambigouity.

Semicolons are not placed at the end of statements, but instead they delimit blocks, such as body of a function or a loop. There are no curly brackets. They are not necessary if you, for example, declare an external method or have an `if` expression in a single line. Round brackets `()` are also not ncessary in most of the cases, for example when defining a function or evaluating a condition, but are required for function calls. New lines also play important role and may be required or invalid, depending on the context.

Single equal sign `=` denotes comparion and instead left arrow `<-` is used as an assign symbol.

Source code is grouped into named modules, each module can be compromised of number of files. There is no separate header file, instead prefix `@pub` if attached to externally visible symbols.

## Language Elements
- Comments (`//`, `/* */`)
- Literals (`123`, `0xa2`, `0b0101`, `3.14`, `"Hello"`, `'!'`, `true`, `false`)
- Operators (`+`, `-`, `*`, `/`, `%`, `<-`, `<`, `<=`, `>`, `>=`, `=`, `!=`)
- Logical Operators (`or`, `xor`, `and`, `not`)
- Variables (`u8`, `u32`, `s8`, `s32`, `r32`, `data`, `blob`)
- Functions (`fun`)
- Raw Functions (`raw`)
- Conditional Expressions (`if`, `else`)
- Loops (`rep`)
- Modules (`@module`, `@import`, `@export`, `@extern`)

## Comments
Like in C, comments can specified using either `\\` which will run until the end of the line or through `/* */` block. However, unlike C, the `/* bla bla /* bla */ */` comments can be also embeded inside each other.
```
// this is a main function
main fun -> u32
  /*
    num1 <- 2 + 5
    /* num1 <- 4 * num1 */
    // num1 <- 5 * num1
  */
  ret 0
;

```

## Literals
**Number literals** can be specified as decimal, hexadecimal, and binary numbers. Digits can be separated by an '_' but it cannot be the first or the last character (otherwise it will get interpreted as an identifier).

```
// Valid examples:
1024
1_024
1.245

1_000.0

0xffa
0xffaa_42bb

0b1101
0b1010_0101

// Invalid examples:
_100
1000_.100
3.
3._14

0x_fa

0b10_
_0b1101
```

**Text literals** can be specified either as an implicitly zero terminated string, or as a single character. Strings are converted into arrays. Characters can be also backslash '\' escaped, just like in C.
```
// Examples
"Hello world"
"Hello world\0" // in this case, the final zero is not appended
'H'
'!'
'\n'

// Escape sequences
'\b' // backspace
'\n' // new line
'\t' // tab
'\\' // backslash
'\'' // single quotation mark
'\"' // double quotiation mark
'\0' // 0 (as in integer 0)
```

**Boolean literals** can also be specified using `true` or `false` keywords. There is no implicit conversion from integer to boolean and vice-versa.

## Operators
All the standard operators, such as `+`, `-`, `*`, `/`, `%` are available. The biggest difference is that the assignment uses the left arrow `<-` and thus a comparison can be done through a single equal sign `=`.
```
+ // addition
- // subtraction
* // multiplication
/ // division
% // division reminder
= // is equal
!= // is not equal
< // is less than
<= // is less or equal
=> // is greater than
> // is gearter or equal
<- // assignment
( ) // precdence
```

## Logical Operators
There's a couple of standard logical operations that can be performed on bool values.
```
not // highest priority
and // lower priority
or, xor // lowest piority
```
`=` and `!=` can also be used on booleans, but they are effectively equivalent to `and` and `xor`.

## Variables
Variables are specified by first providing the name and then the type. There is also an optional initializer.
```
bytesInKilobyte u32 <- 1_024
text data<u8> <- "Hello world!"
pi r32 <- 3.14
```

**Simple variables**
There are standard float and integer types available, but unlike in C, you have to be explicit about their size and signiness. You can only perform `=` and `!=` operations on booleans. There is no `void` type or an equivalent.
```
u8 // unsigned integer, 8 bits
u32 // unsigned integer, 32 bits
s8 // signed integer, 8 bits
s32 // signed integer, 32 bits
r32 // floating point (real), 32 bits
bool // true or false
```

**Data variables** or arrays, as known in C. They are a sequence of static length or elements of the same type. Length has to be specified either explicitly or through and initializer.
```
text data<u8> <- "Hello world!"
fibonaciNumbers<u32, 4> <- [1, 1, 2, 3, 5, 8] // Anything past the first 4 numbers will be ignored
```

**Blob variables**, otherwise known as structures. Composite types which we can specify by ourselves. The usage is fairly smillar as in C. Semicolon and new line are required in the definition.
```
user blob
  age u32
  name data<u8, 10>
  isActive bool
;

bob user
bob.age <- 18
bob.name <- "Bob"
bob.isActive <- true
```

## Functions
Functions in BRC work just like in C. You can specify an optional list of arguments and a return type. Calls require usage of round brackets. Colon should be omitted if there are no arguments. Arrow has to be on the same line as the return type.
```
// Valid examples
main fun -> u32
  ret 0
;

addNums fun: num1 s32, s32 -> s32
  ret num1 + num2
;

addNums fun:
  num1 s32,
  num2 s32
  -> s32

  ret num1 + num2
;

addNums(5, 4)

logInUser fun: user User
  // do some internet stuff 📡
;

logInUser(bob)

explodeEverything fun
  // Do a boom! 💥
;

explodeEverything()

// Invalid examples
addNums num1 s32, num2 s32 -> s32
[..]

addNums: num1 s32
  ,num2 s32 -> s32
[..]

addNums: num1 s32, num2 s32 ->
  s32
[..]
```

## Raw Functions
A unique feature of BRC is a seamless use of inline assembly. Raw functions can be used just like normal functions, altoght there is a couple of limitations and they require so called constraints to be specified. It's the same as in gcc or clang, but they are specified as a single string instead of splitting them into input, output, and clobbers. Some more information can be found here [A Practical Guide to GCC Inline Assembly](https://blog.alex.balgavy.eu/a-practical-guide-to-gcc-inline-assembly/). Intel syntax is used for the assembly.
```
rawAdd raw<"=r,r,r">: num1 u32, num2 u32 -> u32
    add $1, $2
    mov $0, $1
;

// later on
result u32 <- rawAdd(5, 4)
```

## Conditional Expressions
If-Else statements can be written on a single or multiple lines and are an expression, which allows them to return values.
```
isValid bool <- if count = 0: doForEmpty() else doForCount(count)

if numer > 0:
  doStuff(number)
else
  fatalError()
;

if featureFlag:
  // Special case ⚰️
;

if hasCompleted: exit()

if processedElementsCount < 10: print("Success) else
  print("Failure")
  processFailure(processedElementsCount)
;
```

## Loops
C-style for, while, and do-while are all combined into a single `rep` loop. The format is `rep init, pre-condition, post-condition`. `init` allows to setup a counter, pre-condition is evaluated before and post after each loop. Each part is optional, but if you include post-condition, pre-condition must also be include. Body can be specified on the same line as the loop, in which case the final semicolon should not be included.
```
// infinite loop
rep: doStuff()

// do things ten times
rep i u32 <- 0, i < 10:
  doStuff(i)
  i <- i + 1
;

// do things at least once
rep i u32 <- 0, true, i < someValue:
  doStuff(i)
;
```

## Modules
Each source forms a module and each module can be made out of multiple source files. `@module someModule` must be placed at the beginning of the file. If no `@module` is specified, then `@module main` is assumed. Main module lives in the global namespace.

Each module can export functionality using the `@export` prefix. In order to use a different module it has to be iported with `@import` and then we can use its exported symbols by prefixing them with `@moduleName.`.

```
// app.brc
// @module main is assumed
@import console

// we need to export main so it's available to the OS
@export main fun -> u32
  @console.print("Hello, world!)
;


// console.brc
@module console

@export print fun: text data<u32, 32> 
  [..]
;
```

We can then build both of the sources together with `brb app.brc console.brc` which will produce object files `main.o` and `console.o`.

Separately linked symbosl can be specified using `@extern`. For example, we we want to use something from the standard library we can use `@extern putchar fun: character u32 -> u32`, which can then be resolved by linker.
