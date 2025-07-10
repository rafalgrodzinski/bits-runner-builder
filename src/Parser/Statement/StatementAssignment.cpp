#include "StatementAssignment.h"

StatementAssignment::StatementAssignment(string name, shared_ptr<Expression> indexExpression, shared_ptr<Expression> expression):
Statement(StatementKind::ASSIGNMENT), name(name), indexExpression(indexExpression), expression(expression) { }

string StatementAssignment::getName() {
    return name;
}

shared_ptr<Expression> StatementAssignment::getIndexExpression() {
    return indexExpression;
}

shared_ptr<Expression> StatementAssignment::getExpression() {
    return expression;
}