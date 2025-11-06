#ifndef EXPRESSION_GROUPING_H
#define EXPRESSION_GROUPING_H

#include "Expression.h"

class ExpressionGrouping: public Expression {
private:
    shared_ptr<Expression> subExpression;

public:
    ExpressionGrouping(shared_ptr<Expression> subExpression, int line, int column);
    shared_ptr<Expression> getSubExpression();
};

#endif