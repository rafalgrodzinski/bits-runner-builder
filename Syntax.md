# Detailed Syntax

This documents specifies what is the allowed syntax for statements and expressions.

### Symbols used
`?` 0 or 1 instances

`*` 0 or more instance

`+` 1 or more instances

`<NL>` New line

`<ID>` Identifier

`<TYPE>` Type

### Overall structure
```
<Statement Meta Module>
|
+ <Statement Meta Extern Function>
+ <Statemnet Variable>
+ <Statement Function>
  |
  + <Statement Block>
    |
    + <Statemnet Variable>
    + <Statement Assignment>
    + <Statement Return>
    + <Statement Repeat>
      |
      + <Statement Block>...
    + <Statement Expression>
      |
      + <Expression>
      + <Expression If-Else>
        |
        + <Expression Block>...
```

### Statement Meta Extern Function
`@extern <ID> fun (: <NL>? <ID> <TYPE> (, <NL>? <ID> <TYPE>)*)? (-> <NL>? <TYPE>)?`
```
@extern sum fun:
num1 sint32,
num2 sint32 ->
sint32
```

### Statement Variable
`<ID> <TYPE> <- <Expression>`

### Statement Function
`<ID> fun (: <NL>? <ID> <TYPE> (, <NL>? <ID> <TYPE>)*)? (-> <NL>? <TYPE>)? <NL> <Statement Block> ;`
```
stuff fun
;

stuff fun -> sint32
    ret 42
;

stuff fun: num1 sint32,
num2 sint32 ->
sint32
    ret num1 + num2
;
```

### Statement Assignment
`<ID> <- <Expression>`
```
num1 <- 5
```

### Statement Block
`(<Statement> <NL>)*`

### Statement Repeat
`rep [<StatementVariable> | <StatementAssignment>]? : <Statement Block>`

`rep [<StatementVariable> | <StatementAssignment>]? : <NL> <Statement Block> ;`

`rep [<StatementVariable> | <StatementAssignment>] (, <NL>? <Expression> (, <NL>? <Expression>)? )? : <StatementBlock>`

`rep [<StatementVariable> | <StatementAssignment>] (, <NL>? <Expression> (, <NL>? <Expression>)? )? : <NL> <StatementBlock> ;`

`rep (<Expression> (, <NL>? <Expression>)? )? : <StatementBlock>`

`rep (<Expression> (, <NL>? <Expression>)? )? : <NL> <StatementBlock> ;`
```
rep i sint32 <- 0, true, i < 10:
    doStuff(i)
;

rep i sint32 <- 0,
true, i < 10:
    doStuff(i)
;

rep i sint32 <- 0,
true,
i < 10:
    doStuff(i)
;

rep: infiniteCall()
```

### Statement Return
`ret <Expression>?`

### Expression Variable
`<ID>`

### Expression If-Else:
`if <Expression> : <ExpressionBlock>`
```
if num1 > 10: putchar('T')

```

`if <Expression> : <NL> <ExpressionBlock> ;`
```
if num1 > 10:
    num1 <- 500
    putchar('S')
;
```

`if <Expression> : <ExpressionBlock> else <ExpressionBlock>`
```
if num1 > 10: putchar('T') else putchar('N')

```

`if <Expression> : <NL> <ExpressionBlock> else <NL> <ExpressionBlock> ;`
```
if num1 > 10:
    putchar('T')
else
    putchar('N')
;

```

`if <Expression> : <ExpressionBlock> else <NL> <ExpressionBlock> <NL> ;`
```
if num1 > 10: putchar('T') else
    num1 <- 500
    putchar('F')
;

```

### Expression Block
`<Statement> <NL>)* <Statement Expression>?`