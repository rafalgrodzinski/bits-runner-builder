#ifndef STATEMENT_ASSIGNMENT_H
#define STATEMENT_ASSIGNMENT_H

#include "Statement.h"

class Expression;

class StatementAssignment: public Statement {
private:
    vector<shared_ptr<Expression>> chainExpressions;
    shared_ptr<Expression> valueExpression;

public:
    StatementAssignment(vector<shared_ptr<Expression>> chainExpressions, shared_ptr<Expression> valueExpression, int line, int column);
    vector<shared_ptr<Expression>> getChainExpressions();
    shared_ptr<Expression> getValueExpression();
};

#endif