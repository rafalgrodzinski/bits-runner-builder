#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include "Token.h"
#include "Expression.h"

class Parser {
private:
    std::vector<Token> tokens;
    int currentIndex = 0;

    //Expression term();
    //Expression primary();

    Expression matchInteger();

public:
    Parser(std::vector<Token> tokens);
    //Expression getExpression();
};

#endif