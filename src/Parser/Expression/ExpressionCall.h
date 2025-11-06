#ifndef EXPRESSION_CALL_H
#define EXPRESSION_CALL_H

#include "Expression.h"

class ExpressionCall: public Expression {
private:
    string name;
    vector<shared_ptr<Expression>> argumentExpressions;

public:
    ExpressionCall(string name, vector<shared_ptr<Expression>> argumentExpressions, int line, int column);
    string getName();
    vector<shared_ptr<Expression>> getArgumentExpressions();
};

#endif