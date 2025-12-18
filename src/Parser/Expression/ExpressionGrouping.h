#ifndef EXPRESSION_GROUPING_H
#define EXPRESSION_GROUPING_H

#include "Expression.h"

class ExpressionGrouping: public Expression {
private:
    shared_ptr<Expression> subExpression;

public:
    ExpressionGrouping(shared_ptr<Expression> subExpression, shared_ptr<Location> location);
    shared_ptr<Expression> getSubExpression();
};

#endif