#ifndef EXPRESSION_IF_ELSE_H
#define EXPRESSION_IF_ELSE_H

#include "Expression.h"

class ExpressionIfElse: public Expression {
friend class TypesAnalyzer;

private:
    shared_ptr<Expression> conditionExpression;
    shared_ptr<Expression> thenExpression;
    shared_ptr<Expression> elseExpression;

public:
    ExpressionIfElse(shared_ptr<Expression> conditionExpression, shared_ptr<Expression> thenExpression, shared_ptr<Expression> elseExpression, int line, int column);
    shared_ptr<Expression> getConditionExpression();
    shared_ptr<Expression> getThenExpression();
    shared_ptr<Expression> getElseExpression();
};

#endif