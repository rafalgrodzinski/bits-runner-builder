#include "ExpressionVariable.h"

shared_ptr<ExpressionVariable> ExpressionVariable::simple(string identifier, int line, int column) {
    shared_ptr<ExpressionVariable> expression = make_shared<ExpressionVariable>(line, column);
    expression->variableKind = ExpressionVariableKind::SIMPLE;
    expression->identifier = identifier;
    return expression;
}

shared_ptr<ExpressionVariable> ExpressionVariable::data(string identifier, shared_ptr<Expression> indexExpression, int line, int column) {
    shared_ptr<ExpressionVariable> expression = make_shared<ExpressionVariable>(line, column);
    expression->variableKind = ExpressionVariableKind::DATA;
    expression->identifier = identifier;
    expression->indexExpression = indexExpression;
    return expression;
}

ExpressionVariable::ExpressionVariable(int line, int column):
Expression(ExpressionKind::VARIABLE, nullptr, line, column) { }

ExpressionVariableKind ExpressionVariable::getVariableKind() {
    return variableKind;
}

string ExpressionVariable::getIdentifier() {
    return identifier;
}

shared_ptr<Expression> ExpressionVariable::getIndexExpression() {
    return indexExpression;
}