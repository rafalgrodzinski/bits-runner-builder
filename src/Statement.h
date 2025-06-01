#ifndef STATEMENT_H
#define STATEMENT_H

#include <iostream>

#include "Token.h"
#include "Expression.h"

using namespace std;

class Statement {
public:
    enum Kind {
        EXPRESSION,
        INVALID
    };

private:
    Kind kind;
    Token token;
    shared_ptr<Expression> expression;

public:
    Statement(Kind kind, Token token, shared_ptr<Expression> expression);
    Token getToken();
    shared_ptr<Expression> getExpression();
    bool isValid();
    string toString();
};

#endif