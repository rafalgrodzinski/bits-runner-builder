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
public:
    static std::shared_ptr<ExpressionUnary> expression(std::shared_ptr<Token> token, std::shared_ptr<Expression> subExpression);

    ExpressionUnary(std::shared_ptr<Location> location);

    ExpressionUnaryOperation getOperation() const;
    std::shared_ptr<Expression> getSubExpression() const;

private:
    ExpressionUnaryOperation operation;
    std::shared_ptr<Expression> subExpression;
};

#endif