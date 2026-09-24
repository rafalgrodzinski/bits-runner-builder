#ifndef EXPRESSION_GROUPING_H
#define EXPRESSION_GROUPING_H

#include "Expression.h"

class ExpressionGrouping: public Expression {
public:
    ExpressionGrouping(std::shared_ptr<Expression> subExpression, std::shared_ptr<Location> location);

    std::shared_ptr<Expression> getSubExpression() const;

private:
    std::shared_ptr<Expression> subExpression;
};

#endif