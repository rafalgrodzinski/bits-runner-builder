#ifndef STATEMENT_ASSIGNMENT_CHAINED_H
#define STATEMENT_ASSIGNMENT_CHAINED_H

#include "Statement.h"

class Expression;
class ExpressionChained;

class StatementAssignmentChained: public Statement {
private:
    shared_ptr<ExpressionChained> chainExpression;
    shared_ptr<Expression> valueExpression;

public:
    StatementAssignmentChained(shared_ptr<ExpressionChained> chainExpression, shared_ptr<Expression> valueExpression);
    shared_ptr<ExpressionChained> getChainExpression();
    shared_ptr<Expression> getValueExpression();
};

#endif