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

public:
    static std::shared_ptr<ExpressionBinary> expression(const std::vector<std::shared_ptr<Token>> &tokens, std::shared_ptr<Expression> left, std::shared_ptr<Expression> right);
    static std::shared_ptr<ExpressionBinary> expression(ExpressionBinaryOperation operation, std::shared_ptr<Expression> left, std::shared_ptr<Expression> right, std::shared_ptr<Location> location);

    ExpressionBinary(std::shared_ptr<Location> location);

    ExpressionBinaryOperation getOperation() const;
    std::shared_ptr<Expression> getLeft() const;
    std::shared_ptr<Expression> getRight() const;

private:
    ExpressionBinaryOperation operation;
    std::shared_ptr<Expression> left;
    std::shared_ptr<Expression> right;

    static bool doTokensMatchTokenKinds(const std::vector<std::shared_ptr<Token>> &tokens, const std::vector<TokenKind> &tokenKinds);
};

#endif