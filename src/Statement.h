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
        BLOCK,
        FUNCTION_DECLARATION,
        INVALID
    };

private:
    Kind kind;
    shared_ptr<Token> token;
    shared_ptr<Expression> expression;
    shared_ptr<Statement> blockStatement;
    vector<shared_ptr<Statement>> statements;
    string name;

public:
    Statement(Kind kind, shared_ptr<Token> token, shared_ptr<Expression> expression, shared_ptr<Statement> blockStatement, vector<shared_ptr<Statement>> statements, string name);
    shared_ptr<Token> getToken();
    shared_ptr<Expression> getExpression();
    bool isValid();
    string toString();
};

#endif