#ifndef EXPRESSION_IF_ELSE_H
#define EXPRESSION_IF_ELSE_H

#include "Expression.h"

class ExpressionBlock;

class ExpressionIfElse: public Expression {
private:
    shared_ptr<Expression> condition;
    shared_ptr<ExpressionBlock> thenBlock;
    shared_ptr<Expression> elseExpression;

public:
    ExpressionIfElse(shared_ptr<Expression> condition, shared_ptr<ExpressionBlock> thenBlock, shared_ptr<Expression> elseExpression);
    shared_ptr<Expression> getCondition();
    shared_ptr<ExpressionBlock> getThenBlock();
    shared_ptr<Expression> getElseExpression();
};

#endif