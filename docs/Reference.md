# BRC Language Reference

## Overview
Bits Runner Code (BRC) borrows a lot of concepts and syntax from C, but in a slightly modified way. The idea is to use familiar concept in a simplified way, avoiding usage of unnecessary fluff and just to make the code simpler and shorter, while avoiding any unambigouity.

Semicolons are not placed at the end of statements, but instead they delimit blocks, such as body of a function or a loop. There are no curly brackets. They are not necessary if you, for example, declare an external method or have an `if` expression in a single line. Round brackets `()` are also not ncessary in most of the cases, for example when defining a function or evaluating a condition, but are required for function calls. New lines also play important role and may be required or invalid, depending on the context.

Single equal sign `=` denotes comparion and instead left arrow `<-` is used as an assign symbol.

Source code is grouped into named modules, each module can be compromised of number of files. There is no separate header file, instead prefix `@pub` if attached to externally visible symbols.

## Language Elements
- [Comments](Reference.md#comments) (`//`, `/* */`)
- [Literals](Reference.md#literals) (`123`, `0xa2`, `0b0101`, `3.14`, `"Hello"`, `'!'`, `true`, `false`)
- [Operators](Reference.md#operators) (`+`, `-`, `*`, `/`, `%`, `<-`, `<`, `<=`, `>`, `>=`, `=`, `!=`)
- [Logical Operators](Reference.md#logical-operators) (`or`, `xor`, `and`, `not`)
- [Simple Variables](Reference.md#simple-variables) (`u8`, `u32`, `u64`, `s8`, `s32`, `s64`, `f32`, `f64`, `data`, `blob`, `ptr`)
- [Data](Reference.md#data) (`data<>`)
- [Blob](Reference.md#blob) (`blob<>`)
- [Pointers](Reference.md#pointers) (`ptr<>`)
- [Functions](Reference.md#functions) (`fun`)
- [Raw Functions](Reference.md#raw-functions) (`raw`)
- [Conditional Expressions](Reference.md#conditional-expressions) (`if`, `else`)
- [Repeats](Reference.md#repeats) (`rep`)
- [Chaining](Reference.md#chaining) (`thing.sutff[5].something`)
- [Modules](Reference.md#modules) (`@module`, `@import`, `@export`, `@extern`)

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
( ) // precedence
```

## Logical Operators
There's a couple of standard logical operations that can be performed on bool values.
```
not // highest priority
and // lower priority
or, xor // lowest piority
```
`=` and `!=` can also be used on booleans, but they are effectively equivalent to `and` and `xor`.

## Simple Variables
Simple ariables are specified by first providing the name and then the type. They can have an optional initializer. There are standard float and integer types available, but unlike in C, you have to be explicit about their size and signiness. You can only perform `=` and `!=` operations on booleans. There is no `void` type or an equivalent.
```
bytesInKilobyte u32 <- 1_024
text data<u8> <- "Hello world!"
pi f32 <- 3.14

u8 // unsigned integer, 8 bits
u32 // unsigned integer, 32 bits
s8 // signed integer, 8 bits
s32 // signed integer, 32 bits
f32 // floating point, 32 bits
bool // true or false
```

## Data
Data is a composite of identical members and constant size, also known as arrays. It has a built-in member `.count`, which is equivalent to the number of elements of the array. An array can be assigned to another array of the same type, which will create a shallow copy. If their sizes don't mach, only number of elements equivalent to the smaller array will be copied. If size is not specified, it will be inferred either from composite literal, other data variable, or a function call.
```
text <u32> <- "Hello, world!"
rep i u32 <- 0, text[i] != '\0'
  printChar(text[i])
;

numbers <u32, 24> <- {..}
copiedNumbers <u32, 8> <- numbers // Only 8 values will be copied
```

## Blob
Blobs are composites of different member types, also known as structs. Before use they need to be specified using the `blob` keyword. They can be instantiated using the composite literal `{ }` or by assigning each member `.member` individually. Assigning one blob to another will create its copy. There is no casting between different blob types. Blobs can contain other blobs, but only if they have been already beforehand defined. This helps prevent a blobacalypse where `blob1` would contain `blob2` and `blob2` would contain `blob1` which would end up with infinite blobs. Using pointers is fine though.
```
user blob
  id u64
  name data<u8, 16>
;

u1 blob<user> <- {34, "Bob"}
u2 blob<user> <- u1
u2.name <- "Alice"
u2.id <- 35
```

## Pointers
Pointers, just like in C, allow for low-level data manipulation and passing. They have an associated type, which is essential when reading/writing pointee's value. Each variable has a built-in member `.adr`, which will provide a system-dependant (32bit or 64bit) memory address value. Pointers have also `.val`, which is equivalent to its pointee and `vAdr`, which is the address of the thing it is pointing at. Don't confuse `.vAdr` with `.adr`, as the later is the address of the pointer itself.
```
a u32 <- 5
pA ptr<u32>
pA.vAdr <- a.adr // now both a & pA.val indicate the same value
pA.val <- 6
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
If-Else statement can be written on a single or multiple lines. It is an expression, which allows it to return values. If just an `if` is used, a value cannot be returned. Single line version uses colons `:`, which are ommited in the multi-line version.
```
isValid bool <- if count = 0: doForEmpty() else: doForCount(count)

if numer > 0
  doStuff(number)
else
  fatalError()
;

if featureFlag
  // Special case ⚰️
;

if hasCompleted: exit()

if processedElementsCount < 10
  print("Success)
else
  print("Failure")
  processFailure(processedElementsCount)
;
```

## Repeats
C-style for, while, and do-while are all combined into a single `rep` loop. The format is `rep init-statement, pre-condition, post-condition, post-statement`. `init-statement` allows to setup a counter, pre-condition is evaluated before and post after each loop. `post-statement` is evaluated at the end of each loop. Each part is optional, but if you include post-condition, pre-condition must also be include. If you include `post-statement` then `init-statement` also has to be included. Body can be specified on the same line as the loop, in which case the final semicolon should be also included.
```
// infinite loop
rep: doStuff()

// do things ten times
rep i u32 <- 0, i < 10, i <- i + 1
  doStuff(i)
;

// do things at least once
rep i u32 <- 0, true, i < someValue: doStuff(i)
```

## Chaining
Expressions and assignment statments can be chained, which allows for some complex logic to be created in a clearer way. This is Especially useful when dealing with pointers.
```
tree blob
    branches data<u64, 8>
;

forest blob
    name data<u64, 16>
    trees data<blob<tree>, 8>
;

niceOne blob<forest>
niceOne.name <- "Nice one!"
niceOne.trees[0].branches[0] <- 1
niceOne.trees[0].branches[1] <- 2
niceOne.trees[0].branches[2] <- 4

//

thing blob
  id u64
;

something blob<thing> <- { 7 }
pThing ptr<blob<thing>> <- { thing.adr }
pThing.val.id <- 8
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
