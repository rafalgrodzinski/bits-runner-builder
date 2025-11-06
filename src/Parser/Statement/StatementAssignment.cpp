#include "StatementAssignment.h"

StatementAssignment::StatementAssignment(vector<shared_ptr<Expression>> chainExpressions, shared_ptr<Expression> valueExpression, int line, int column):
Statement(StatementKind::ASSIGNMENT, line, column), chainExpressions(chainExpressions), valueExpression(valueExpression) { }

vector<shared_ptr<Expression>> StatementAssignment::getChainExpressions() {
    return chainExpressions;
}

shared_ptr<Expression> StatementAssignment::getValueExpression() {
    return valueExpression;
}