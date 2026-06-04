#ifndef EXPRESSION_CALL_H
#define EXPRESSION_CALL_H

#include "Expression.h"

class ExpressionCall: public Expression {
friend class Analyzer;

private:
    string name;
    vector<shared_ptr<Expression>> argumentExpressions;

public:
    ExpressionCall(const string &name, const vector<shared_ptr<Expression>> &argumentExpressions, shared_ptr<Location> location);

    string getName() const;
    vector<shared_ptr<Expression>> getArgumentExpressions() const;
};

#endif