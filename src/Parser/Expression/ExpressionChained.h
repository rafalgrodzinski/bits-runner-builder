#ifndef EXPRESSION_CHAINED_H
#define EXPRESSION_CHAINED_H

#include "Expression.h"

class ExpressionChained: public Expression {
public:
    ExpressionChained(const std::vector<std::shared_ptr<Expression>> &chainExpressions, std::shared_ptr<Location> location);

    std::vector<std::shared_ptr<Expression>> getChainExpressions() const;

private:
    std::vector<std::shared_ptr<Expression>> chainExpressions;
};

#endif