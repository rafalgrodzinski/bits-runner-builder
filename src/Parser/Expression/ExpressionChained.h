#ifndef EXPRESSION_CHAINED_H
#define EXPRESSION_CHAINED_H

#include "Expression.h"

class ExpressionChained: public Expression {
private:
    shared_ptr<ExpressionChained> parentExpression;
    shared_ptr<Expression> expression;

public:
    ExpressionChained(shared_ptr<ExpressionChained> parentExpression, shared_ptr<Expression> expression);
    shared_ptr<ExpressionChained> getParentExpression();
    shared_ptr<Expression> getExpression();
};

#endif