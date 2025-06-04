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

private:
    Kind kind;

public:
    Expression(Kind kind);
    Kind getKind();
    bool isValid();
    virtual string toString();
};

class ExpressionLiteral: public Expression {
private:
    int64_t integer = 0;

public:
    ExpressionLiteral(shared_ptr<Token> token);
    int64_t getInteger();
    string toString() override;
};

class ExpressionGrouping: public Expression {
private:
    shared_ptr<Expression> expression;

public:
    ExpressionGrouping(shared_ptr<Expression> expression);
    shared_ptr<Expression> getExpression();
    string toString() override;
};

class ExpressionBinary: public Expression {
public:
    enum Operation {
        ADD,
        SUB,
        MUL,
        DIV,
        MOD
    };

private:
    Operation operation;
    shared_ptr<Expression> left;
    shared_ptr<Expression> right;

public:
    ExpressionBinary(shared_ptr<Token> token, shared_ptr<Expression> left, shared_ptr<Expression> right);
    Operation getOperation();
    shared_ptr<Expression> getLeft();
    shared_ptr<Expression> getRight();
    string toString() override;
};

class ExpressionInvalid: public Expression {
private:
    shared_ptr<Token> token;

public:
    ExpressionInvalid(shared_ptr<Token> token);
    shared_ptr<Token> getToken();
    string toString() override;
};

#endif