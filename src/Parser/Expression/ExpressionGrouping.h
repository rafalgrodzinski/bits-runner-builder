#ifndef EXPRESSION_GROUPING_H
#define EXPRESSION_GROUPING_H

#include "Expression.h"

class ExpressionGrouping: public Expression {
private:
    shared_ptr<Expression> expression;

public:
    ExpressionGrouping(shared_ptr<Expression> expression);
    shared_ptr<Expression> getExpression();
};

#endif