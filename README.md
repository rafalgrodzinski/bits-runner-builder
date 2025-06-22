# Bits Runner Builder

## Overview
Bits Runner Builder (brb) is a compiler for Bits Runner Code (brc) language, which has been designed for the Bits Runner Builder operating system. It aims to be a low-level language, which can be a replacement for C while providing a revised syntax and a couple of quality of life improvement. It's a simple system programming language, so no class hierarchies, templates, or other unnecessary fluff.

It has been been built on top of LLVM.

## Show me the code!

### Comments
Like in C, comments can specified using either `\\` which will run until the end of the line or through `/* */` block. However, unlike C, the `/* bla bla /* bla */ */` comments can be also embeded inside each other.

### Literals
**Number literals** can be specified as decimal, hexadecimal, and binary numbers. Digits can be separated by an '_' but it cannot be the first or the last character (otherwise it will get interpreted as and identifier).

```
// Valid examples:
1024
1_024
1.245

1_000.

0xffa
0xffaa_42bb

0b1101
0b1010_0101

// Invalid examples:
_100
1000_.100

0x_fa

0b10_
_0b1101
```

### Control flow

### Functions
