#include "ExpressionValue.h"

shared_ptr<ExpressionValue> ExpressionValue::simple(string identifier, int line, int column) {
    shared_ptr<ExpressionValue> expression = make_shared<ExpressionValue>(line, column);
    expression->valueKind = ExpressionValueKind::SIMPLE;
    expression->identifier = identifier;
    return expression;
}

shared_ptr<ExpressionValue> ExpressionValue::data(string identifier, shared_ptr<Expression> indexExpression, int line, int column) {
    shared_ptr<ExpressionValue> expression = make_shared<ExpressionValue>(line, column);
    expression->valueKind = ExpressionValueKind::DATA;
    expression->identifier = identifier;
    expression->indexExpression = indexExpression;
    return expression;
}

ExpressionValue::ExpressionValue(int line, int column):
Expression(ExpressionKind::VALUE, nullptr, line, column) { }

ExpressionValueKind ExpressionValue::getValueKind() {
    return valueKind;
}

string ExpressionValue::getIdentifier() {
    return identifier;
}

shared_ptr<Expression> ExpressionValue::getIndexExpression() {
    return indexExpression;
}