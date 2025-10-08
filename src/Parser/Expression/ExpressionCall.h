#ifndef EXPRESSION_CALL_H
#define EXPRESSION_CALL_H

#include "Expression.h"

class ExpressionCall: public Expression {
private:
    string name;
    vector<shared_ptr<Expression>> argumentExpressions;

public:
    ExpressionCall(string name, vector<shared_ptr<Expression>> argumentExpressions);
    string getName();
    vector<shared_ptr<Expression>> getArgumentExpressions();
};

#endif