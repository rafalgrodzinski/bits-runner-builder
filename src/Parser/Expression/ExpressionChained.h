#ifndef EXPRESSION_CHAINED_H
#define EXPRESSION_CHAINED_H

#include "Expression.h"

class ExpressionChained: public Expression {
private:
    vector<shared_ptr<Expression>> chainExpressions;

public:
    ExpressionChained(vector<shared_ptr<Expression>> chainExpressions, int line, int column);
    vector<shared_ptr<Expression>> getChainExpressions();
};

#endif