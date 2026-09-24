#ifndef EXPRESSION_IF_ELSE_H
#define EXPRESSION_IF_ELSE_H

#include "Expression.h"

class ExpressionIfElse: public Expression {
friend class Analyzer;

public:
    ExpressionIfElse(shared_ptr<Expression> conditionExpression, shared_ptr<Expression> thenExpression, shared_ptr<Expression> elseExpression, shared_ptr<Location> location);

    shared_ptr<Expression> getConditionExpression() const;
    shared_ptr<Expression> getThenExpression() const;
    shared_ptr<Expression> getElseExpression() const;

private:
    shared_ptr<Expression> conditionExpression;
    shared_ptr<Expression> thenExpression;
    shared_ptr<Expression> elseExpression;
};

#endif