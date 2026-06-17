#ifndef EXPRESSION_CHAINED_H
#define EXPRESSION_CHAINED_H

#include "Expression.h"

class ExpressionChained: public Expression {
private:
    vector<shared_ptr<Expression>> chainExpressions;

public:
    ExpressionChained(const vector<shared_ptr<Expression>> &chainExpressions, shared_ptr<Location> location);

    vector<shared_ptr<Expression>> getChainExpressions() const;
};

#endif