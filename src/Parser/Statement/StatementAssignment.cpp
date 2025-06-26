#include "StatementAssignment.h"

StatementAssignment::StatementAssignment(string name, shared_ptr<Expression> expression):
Statement(StatementKind::ASSIGNMENT), name(name), expression(expression) { }

string StatementAssignment::getName() {
    return name;
}

shared_ptr<Expression> StatementAssignment::getExpression() {
    return expression;
}