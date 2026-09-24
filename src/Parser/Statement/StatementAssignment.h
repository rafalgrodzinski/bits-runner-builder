#ifndef STATEMENT_ASSIGNMENT_H
#define STATEMENT_ASSIGNMENT_H

#include "Statement.h"

class Expression;
class ExpressionChained;

class StatementAssignment: public Statement {
friend class Analyzer;

public:
    StatementAssignment(
        std::shared_ptr<ExpressionChained> expressionChained,
        std::shared_ptr<Expression> valueExpression,
        std::shared_ptr<Location> location
    );

    std::shared_ptr<ExpressionChained> getExpressionChained() const;
    std::shared_ptr<Expression> getValueExpression() const;

private:
    std::shared_ptr<ExpressionChained> expressionChained;
    std::shared_ptr<Expression> valueExpression;
};

#endif