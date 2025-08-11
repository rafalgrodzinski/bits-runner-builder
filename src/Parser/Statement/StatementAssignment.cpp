#include "StatementAssignment.h"

StatementAssignment::StatementAssignment():
Statement(StatementKind::ASSIGNMENT) { }

shared_ptr<StatementAssignment> StatementAssignment::variableAssignment(string identifier, shared_ptr<Expression> valueExpression) {
    shared_ptr<StatementAssignment> statement = make_shared<StatementAssignment>();
    statement->assignmentKind = StatementAssignmentKind::VARIABLE;
    statement->identifier = identifier;
    statement->valueExpression = valueExpression;
    return statement;
}

shared_ptr<StatementAssignment> StatementAssignment::dataAssignment(string identifier, shared_ptr<Expression> indexExpression, shared_ptr<Expression> valueExpression) {
    shared_ptr<StatementAssignment> statement = make_shared<StatementAssignment>();
    statement->assignmentKind = StatementAssignmentKind::DATA;
    statement->identifier = identifier;
    statement->indexExpression = indexExpression;
    statement->valueExpression = valueExpression;
    return statement;
}

shared_ptr<StatementAssignment> StatementAssignment::blobAssignment(string identifier, string memberName, shared_ptr<Expression> valueExpression) {
    shared_ptr<StatementAssignment> statement = make_shared<StatementAssignment>();
    statement->assignmentKind = StatementAssignmentKind::BLOB;
    statement->identifier = identifier;
    statement->memberName = memberName;
    statement->valueExpression = valueExpression;
    return statement;
}

StatementAssignmentKind StatementAssignment::getAssignmentKind() {
    return assignmentKind;
}

string StatementAssignment::getIdentifier() {
    return identifier;
}

shared_ptr<Expression> StatementAssignment::getIndexExpression() {
    return indexExpression;
}

string StatementAssignment::getMemberName() {
    return memberName;
}

shared_ptr<Expression> StatementAssignment::getValueExpression() {
    return valueExpression;
}