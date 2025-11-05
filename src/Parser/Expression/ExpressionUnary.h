#ifndef EXPRESSION_UNARY_H
#define EXPRESSION_UNARY_H

#include "Expression.h"

enum class ExpressionUnaryOperation {
    NOT,
    BIT_NOT,
    PLUS,
    MINUS
};

class ExpressionUnary: public Expression {
private:
    ExpressionUnaryOperation operation;
    shared_ptr<Expression> subExpression;

public:
    static shared_ptr<ExpressionUnary> expression(shared_ptr<Token> token, shared_ptr<Expression> subExpression);

    ExpressionUnary();

    ExpressionUnaryOperation getOperation();
    shared_ptr<Expression> getSubExpression();
};

#endif