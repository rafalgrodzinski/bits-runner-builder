#ifndef EXPRESSION_IF_ELSE_H
#define EXPRESSION_IF_ELSE_H

#include "Expression.h"

class ExpressionBlock;

class ExpressionIfElse: public Expression {
friend class TypesAnalyzer;

private:
    shared_ptr<Expression> conditionExpression;
    shared_ptr<ExpressionBlock> thenBlockExpression;
    shared_ptr<Expression> elseExpression;

public:
    ExpressionIfElse(shared_ptr<Expression> conditionExpression, shared_ptr<ExpressionBlock> thenBlockExpression, shared_ptr<Expression> elseExpression, int line, int column);
    shared_ptr<Expression> getConditionExpression();
    shared_ptr<ExpressionBlock> getThenBlockExpression();
    shared_ptr<Expression> getElseExpression();
};

#endif