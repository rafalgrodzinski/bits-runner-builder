#include "ExpressionVariable.h"

shared_ptr<ExpressionVariable> ExpressionVariable::simple(string identifier) {
    shared_ptr<ExpressionVariable> expression = make_shared<ExpressionVariable>();
    expression->variableKind = ExpressionVariableKind::SIMPLE;
    expression->identifier = identifier;
    return expression;
}

shared_ptr<ExpressionVariable> ExpressionVariable::data(string identifier, shared_ptr<Expression> indexExpression) {
    shared_ptr<ExpressionVariable> expression = make_shared<ExpressionVariable>();
    expression->variableKind = ExpressionVariableKind::DATA;
    expression->identifier = identifier;
    expression->indexExpression = indexExpression;
    return expression;
}

ExpressionVariable::ExpressionVariable():
Expression(ExpressionKind::VARIABLE, nullptr) { }

ExpressionVariableKind ExpressionVariable::getVariableKind() {
    return variableKind;
}

string ExpressionVariable::getIdentifier() {
    return identifier;
}

shared_ptr<Expression> ExpressionVariable::getIndexExpression() {
    return indexExpression;
}