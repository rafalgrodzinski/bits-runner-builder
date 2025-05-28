#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <Token.h>
#include <Expression.h>

class Parser {
public:
    Parser(std::vector<Token> tokens);
    std::vector<Expression> getExpressions();
};

#endif