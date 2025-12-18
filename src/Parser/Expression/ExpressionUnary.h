#ifndef EXPRESSION_UNARY_H
#define EXPRESSION_UNARY_H

#include "Expression.h"

enum class ExpressionUnaryOperation {
    BIT_NOT,
    MINUS,
    NOT,
    PLUS
};

class ExpressionUnary: public Expression {
private:
    ExpressionUnaryOperation operation;
    shared_ptr<Expression> subExpression;

public:
    static shared_ptr<ExpressionUnary> expression(shared_ptr<Token> token, shared_ptr<Expression> subExpression);

    ExpressionUnary(shared_ptr<Location> location);

    ExpressionUnaryOperation getOperation();
    shared_ptr<Expression> getSubExpression();
};

#endif