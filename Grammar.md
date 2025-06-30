`?` 0 or 1 instances

`*` 0 or more instance

`+` 1 or more instances

`<NL>` New line

`<IDENT>` Identifier

### Statement Function
`<IDENT> fun (: <NL>? <IDENT> <TYPE> (, <NL?> <IDENT> <TYPE>)*)? (-> <NL>? <TYPE>)? <NL> ;`
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
`<IDENT> <- <Expression> <NL>`
```
num1 <- 5

```

### StatementBlock
(<Statement> <NL>)*


### StatementExpression
<Expression> <NL>

StatementFunction:
<IDENT> fun (: <NLO> <IDENT> <TYPE> (, <NLO> <IDENT> <TYPE>)*)? (-> <NLO> <TYPE>)? <NL> 
;

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


StatementMetaExternFunction:
@extern <IDENT> fun (: <NLO> <IDENT> <TYPE> (, <NLO> <IDENT> <TYPE>)*)? (-> <NLO> <TYPE>)? <NL>


### StatementReturn
`ret <Expression>`

### Statemnet Variable
`<IDENT> <TYPE> <- <Expression>`

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