#include "StatementAssignmentChained.h"

StatementAssignmentChained::StatementAssignmentChained(shared_ptr<ExpressionChained> chainExpression, shared_ptr<Expression> valueExpression):
Statement(StatementKind::ASSIGNMENT_CHAINED), chainExpression(chainExpression), valueExpression(valueExpression) { }

shared_ptr<ExpressionChained> StatementAssignmentChained::getChainExpression() {
    return chainExpression;
}

shared_ptr<Expression> StatementAssignmentChained::getValueExpression() {
    return valueExpression;
}