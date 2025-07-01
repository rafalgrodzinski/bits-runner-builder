# Detailed Syntax

This documents specifies what is the allowed syntax for statements and expressions.

### Symbols used
`?` 0 or 1 instances

`*` 0 or more instance

`+` 1 or more instances

`<TER>` Terminal token, usually new line <NL>, but it can also sometimes be `,`, `else`, or `;`

`<NL>` New line

`<ID>` Identifier

`<EXPR_BLOCK>` Expression block

`<STMT_BLOCK>` Statements block

`<EXPR>` Expression

`<STMT>` Statement

### Statement Function
`<ID> fun (: <NL>? <ID> <TYPE> (, <NL>? <ID> <TYPE>)*)? (-> <NL>? <TYPE>)? <NL> <STMT_BLOCK> ; <NL>`
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

### Statement Meta Extern Function:
`@extern <ID> fun (: <NL>? <ID> <TYPE> (, <NL>? <ID> <TYPE>)*)? (-> <NL>? <TYPE>)? <NL>`
```
@extern sum fun:
num1 sint32,
num2 sint32 ->
sint32
```


### Statemnet Variable
`<ID> <TYPE> <- <Expression> <TER>`
```
num sint32 <- 42

```

### Statement Assignment
`<ID> <- <EXPR> <TER>`
```
num1 <- 5

```

### Statement Block
(<Statement> <NL>)*


### Statement Expression
<Expression> <NL>

### StatementLoop
`loop [<StatementVariable> | <StatementAssignment>] (, <NL>? <Expression> (, <NL>? <Expression>)?)? <NL> <StatementBlock> ;`
```
loop i sint32 <- 0, true, i < 10
    doStuff(i)
;

loop i sint32 <- 0,
true, i < 10
    doStuff(i)
;

loop i sint32 <- 0,
true,
i < 10
    doStuff(i)
;
```

`loop (<ExpressionLogical> (, <NL>? <ExpressionLogical>)?)? <NL> <StatementBlock> ;`

### StatementReturn
`ret <Expression>`

### ExpressionVariable:
`<IDENT>`

### Expression If Else:
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
`(<Statement> <NL>)* (<StatementExpression> <NL>?)? <TER>!`