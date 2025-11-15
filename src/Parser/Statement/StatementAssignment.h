#ifndef STATEMENT_ASSIGNMENT_H
#define STATEMENT_ASSIGNMENT_H

#include "Statement.h"

class Expression;
class ExpressionChained;

class StatementAssignment: public Statement {
friend class TypesAnalyzer;

private:
    shared_ptr<ExpressionChained> expressionChained;
    shared_ptr<Expression> valueExpression;

public:
    StatementAssignment(shared_ptr<ExpressionChained> expressionChained, shared_ptr<Expression> valueExpression, int line, int column);
    shared_ptr<ExpressionChained> getExpressionChained();
    shared_ptr<Expression> getValueExpression();
};

#endif