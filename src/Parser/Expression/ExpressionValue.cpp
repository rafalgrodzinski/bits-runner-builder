#include "ExpressionValue.h"

shared_ptr<ExpressionValue> ExpressionValue::simple(string identifier, int line, int column) {
    shared_ptr<ExpressionValue> expression = make_shared<ExpressionValue>(line, column);
    expression->variableKind = ExpressionValueKind::SIMPLE;
    expression->identifier = identifier;
    return expression;
}

shared_ptr<ExpressionValue> ExpressionValue::data(string identifier, shared_ptr<Expression> indexExpression, int line, int column) {
    shared_ptr<ExpressionValue> expression = make_shared<ExpressionValue>(line, column);
    expression->variableKind = ExpressionValueKind::DATA;
    expression->identifier = identifier;
    expression->indexExpression = indexExpression;
    return expression;
}

ExpressionValue::ExpressionValue(int line, int column):
Expression(ExpressionKind::VALUE, nullptr, line, column) { }

ExpressionValueKind ExpressionValue::getVariableKind() {
    return variableKind;
}

string ExpressionValue::getIdentifier() {
    return identifier;
}

shared_ptr<Expression> ExpressionValue::getIndexExpression() {
    return indexExpression;
}