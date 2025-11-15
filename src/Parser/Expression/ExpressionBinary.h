#ifndef EXPRESSION_BINARY_H
#define EXPRESSION_BINARY_H

#include "Expression.h"

enum class ExpressionBinaryOperation {
    OR,
    XOR,
    AND,

    BIT_OR,
    BIT_XOR,
    BIT_AND,
    BIT_SHL,
    BIT_SHR,

    EQUAL,
    NOT_EQUAL,
    LESS,
    LESS_EQUAL,
    GREATER,
    GREATER_EQUAL,

    ADD,
    SUB,
    MUL,
    DIV,
    MOD
};

class ExpressionBinary: public Expression {
friend class TypesAnalyzer;

private:
    ExpressionBinaryOperation operation;
    shared_ptr<Expression> left;
    shared_ptr<Expression> right;

public:
    static shared_ptr<ExpressionBinary> expression(shared_ptr<Token> token, shared_ptr<Expression> left, shared_ptr<Expression> right);

    ExpressionBinary(int line, int column);

    ExpressionBinaryOperation getOperation();
    shared_ptr<Expression> getLeft();
    shared_ptr<Expression> getRight();
};

#endif