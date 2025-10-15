#ifndef EXPRESSION_UNARY_H
#define EXPRESSION_UNARY_H

#include "Expression.h"

enum class ExpressionUnaryOperation {
    NOT,
    BIT_NOT,
    PLUS,
    MINUS,
    INVALID
};

class ExpressionUnary: public Expression {
private:
    ExpressionUnaryOperation operation;
    shared_ptr<Expression> expression;

public:
    ExpressionUnary(shared_ptr<Token> token, shared_ptr<Expression> expression);
    ExpressionUnaryOperation getOperation();
    shared_ptr<Expression> getExpression();
};

#endif