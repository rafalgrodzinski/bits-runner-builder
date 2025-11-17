#include "StatementAssignment.h"

StatementAssignment::StatementAssignment(shared_ptr<ExpressionChained> expressionChained, shared_ptr<Expression> valueExpression, int line, int column):
Statement(StatementKind::ASSIGNMENT, line, column), expressionChained(expressionChained), valueExpression(valueExpression) { }

shared_ptr<ExpressionChained> StatementAssignment::getExpressionChained() {
    return expressionChained;
}

shared_ptr<Expression> StatementAssignment::getValueExpression() {
    return valueExpression;
}