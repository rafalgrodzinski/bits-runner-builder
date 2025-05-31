#ifndef EXPRESSION_H
#define EXPRESSION_H

#include "Token.h"

using namespace std;

class Expression {
public:
    enum Kind {
        LITERAL,
        GROUPING,
        BINARY,
        INVALID
    };

    enum Operator {
        ADD,
        SUB,
        MUL,
        DIV,
        MOD,
        NONE
    };

private:
    Kind kind = INVALID;
    int64_t integer = 0;
    Operator operation = NONE;
    shared_ptr<Expression> left = nullptr;
    shared_ptr<Expression> right = nullptr;

    void setupLiteral(Token token);
    void setupGrouping(Token token, shared_ptr<Expression> expression);
    void setupBinary(Token token, shared_ptr<Expression> left, shared_ptr<Expression> right);

public:
    Expression(Kind kind, Token token, shared_ptr<Expression> left, shared_ptr<Expression> right);
    Kind getKind();
    int64_t getInteger();
    Operator getOperator();
    shared_ptr<Expression> getLeft();
    shared_ptr<Expression> getRight();
    bool operator==(Expression const& other);
    bool operator!=(Expression const& other);
    string toString();

    static shared_ptr<Expression> Invalid;
};

#endif