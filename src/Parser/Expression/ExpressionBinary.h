#ifndef EXPRESSION_BINARY_H
#define EXPRESSION_BINARY_H

#include "Expression.h"

enum class TokenKind;

enum class ExpressionBinaryOperation {
    OR,
    XOR,
    AND,

    BIT_TEST,
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
friend class Analyzer;

private:
    ExpressionBinaryOperation operation;
    shared_ptr<Expression> left;
    shared_ptr<Expression> right;

    static bool doTokensMatchTokenKinds(const vector<shared_ptr<Token>> &tokens, const vector<TokenKind> &tokenKinds);

public:
    static shared_ptr<ExpressionBinary> expression(const vector<shared_ptr<Token>> &tokens, shared_ptr<Expression> left, shared_ptr<Expression> right);
    static shared_ptr<ExpressionBinary> expression(ExpressionBinaryOperation operation, shared_ptr<Expression> left, shared_ptr<Expression> right, shared_ptr<Location> location);

    ExpressionBinary(shared_ptr<Location> location);

    ExpressionBinaryOperation getOperation() const;
    shared_ptr<Expression> getLeft() const;
    shared_ptr<Expression> getRight() const;
};

#endif