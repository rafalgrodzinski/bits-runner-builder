#include "ExpressionVariable.h"

shared_ptr<ExpressionVariable> ExpressionVariable::simpleVariable(string identifier) {
    shared_ptr<ExpressionVariable> expression = make_shared<ExpressionVariable>();
    expression->variableKind = ExpressionVariableKind::SIMPLE;
    expression->identifier = identifier;
    return expression;
}

shared_ptr<ExpressionVariable> ExpressionVariable::dataVariable(string identifier, shared_ptr<Expression> indexExpression) {
    shared_ptr<ExpressionVariable> expression = make_shared<ExpressionVariable>();
    expression->variableKind = ExpressionVariableKind::DATA;
    expression->identifier = identifier;
    expression->indexExpression = indexExpression;
    return expression;
}

shared_ptr<ExpressionVariable> ExpressionVariable::blobVariable(string identifier, string memberName) {
    shared_ptr<ExpressionVariable> expression = make_shared<ExpressionVariable>();
    expression->variableKind = ExpressionVariableKind::BLOB;
    expression->identifier = identifier;
    expression->memberName = memberName;
    return expression;
}

ExpressionVariable::ExpressionVariable():
Expression(ExpressionKind::VAR, nullptr) { }

ExpressionVariableKind ExpressionVariable::getVariableKind() {
    return variableKind;
}

string ExpressionVariable::getIdentifier() {
    return identifier;
}

shared_ptr<Expression> ExpressionVariable::getIndexExpression() {
    return indexExpression;
}

string ExpressionVariable::getMemberName() {
    return memberName;
}