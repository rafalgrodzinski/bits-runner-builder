#ifndef EXPRESSION_IF_ELSE_H
#define EXPRESSION_IF_ELSE_H

#include "Expression.h"

class ExpressionIfElse: public Expression {
friend class Analyzer;

public:
    ExpressionIfElse(std::shared_ptr<Expression> conditionExpression, std::shared_ptr<Expression> thenExpression, std::shared_ptr<Expression> elseExpression, std::shared_ptr<Location> location);

    std::shared_ptr<Expression> getConditionExpression() const;
    std::shared_ptr<Expression> getThenExpression() const;
    std::shared_ptr<Expression> getElseExpression() const;

private:
    std::shared_ptr<Expression> conditionExpression;
    std::shared_ptr<Expression> thenExpression;
    std::shared_ptr<Expression> elseExpression;
};

#endif