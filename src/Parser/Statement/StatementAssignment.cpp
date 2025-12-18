#include "StatementAssignment.h"

StatementAssignment::StatementAssignment(shared_ptr<ExpressionChained> expressionChained, shared_ptr<Expression> valueExpression, shared_ptr<Location> location):
Statement(StatementKind::ASSIGNMENT, location), expressionChained(expressionChained), valueExpression(valueExpression) { }

shared_ptr<ExpressionChained> StatementAssignment::getExpressionChained() {
    return expressionChained;
}

shared_ptr<Expression> StatementAssignment::getValueExpression() {
    return valueExpression;
}