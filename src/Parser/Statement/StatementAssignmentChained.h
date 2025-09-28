#ifndef STATEMENT_ASSIGNMENT_CHAINED_H
#define STATEMENT_ASSIGNMENT_CHAINED_H

#include "Statement.h"

class Expression;

class StatementAssignmentChained: public Statement {
private:
    vector<shared_ptr<Expression>> chainExpressions;
    shared_ptr<Expression> valueExpression;

public:
    StatementAssignmentChained(vector<shared_ptr<Expression>> chainExpressions, shared_ptr<Expression> valueExpression);
    vector<shared_ptr<Expression>> getChainExpressions();
    shared_ptr<Expression> getValueExpression();
};

#endif