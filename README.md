expression := literal | binary | grouping

literal := INTEGER

grouping := "(" expression ")"

binary := expression operator expression

operator := "+" | "-" | "*" | "/" | "%"

expression 
term    factor "+" | "-" factor
factor  primary  "*" | "/" primary
primary INTEGER | "(" expression ")"