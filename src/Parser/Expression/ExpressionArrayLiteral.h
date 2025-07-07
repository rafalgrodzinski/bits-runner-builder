#ifndef EXPRESSION_ARRAY_LITERAL_H
#define EXPRESSION_ARRAY_LITERAL_H

#include "Expression.h"

class ExpressionArrayLiteral: public Expression {
private:
    vector<shared_ptr<Expression>> expressions;

public:
    ExpressionArrayLiteral(vector<shared_ptr<Expression>> expressions);
    vector<shared_ptr<Expression>> getExpressions();
};

#endif