# Fatures Description

## List language elements
- Literals
- Variables
  - Integers (signed & unsigned)
  - Floating points
  - Data (array type)
  - Type (structure type)
  - Pointers
- Functions
- Conditions
- Repeat (loop)
- Modules
- Comments

## Literals
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

## Functions

## Conditions
If-Else statements can be written on a single or multiple lines and are an expression, which allows for something like this:
```
isValid bool <- if count = 0: doForEmpty() else doForCount(count)
```

## Repeats
C-style for, while, and do-while are all combined into a single `rep` loop. The format is `rep init, pre-condition, post-condition`. `init` allows to setup a counter, pre-condition is evaluated before and post after each loop. Each part is optional, but if you include post-condition, pre-condition must also be include. Some examples:
```
// infinite loop
rep: doStuff()

// do things ten times
rep i s32 <- 0, i < 10:
  doStuff(i)
  i <- i + 1
;

// do things at least once
rep i s32 <- 0, true, i < someValue:
  doStuff(i)
;
```

## Comments
Like in C, comments can specified using either `\\` which will run until the end of the line or through `/* */` block. However, unlike C, the `/* bla bla /* bla */ */` comments can be also embeded inside each other.
