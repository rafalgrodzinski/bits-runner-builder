#include "StatementAssignmentChained.h"

StatementAssignmentChained::StatementAssignmentChained(vector<shared_ptr<Expression>> chainExpressions, shared_ptr<Expression> valueExpression):
Statement(StatementKind::ASSIGNMENT_CHAINED), chainExpressions(chainExpressions), valueExpression(valueExpression) { }

vector<shared_ptr<Expression>> StatementAssignmentChained::getChainExpressions() {
    return chainExpressions;
}

shared_ptr<Expression> StatementAssignmentChained::getValueExpression() {
    return valueExpression;
}