#ifndef STATEMENT_ASSIGNMENT_H
#define STATEMENT_ASSIGNMENT_H

#include "Statement.h"

class Expression;
class ExpressionChained;

class StatementAssignment: public Statement {
friend class Analyzer;

public:
    StatementAssignment(
        shared_ptr<ExpressionChained> expressionChained,
        shared_ptr<Expression> valueExpression,
        shared_ptr<Location> location
    );

    shared_ptr<ExpressionChained> getExpressionChained() const;
    shared_ptr<Expression> getValueExpression() const;

private:
    shared_ptr<ExpressionChained> expressionChained;
    shared_ptr<Expression> valueExpression;
};

#endif