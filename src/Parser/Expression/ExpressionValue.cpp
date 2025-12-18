#include "ExpressionValue.h"

shared_ptr<ExpressionValue> ExpressionValue::simple(string identifier, shared_ptr<Location> location) {
    shared_ptr<ExpressionValue> expression = make_shared<ExpressionValue>(location);
    expression->valueKind = ExpressionValueKind::SIMPLE;
    expression->identifier = identifier;
    return expression;
}

shared_ptr<ExpressionValue> ExpressionValue::data(string identifier, shared_ptr<Expression> indexExpression, shared_ptr<Location> location) {
    shared_ptr<ExpressionValue> expression = make_shared<ExpressionValue>(location);
    expression->valueKind = ExpressionValueKind::DATA;
    expression->identifier = identifier;
    expression->indexExpression = indexExpression;
    return expression;
}

ExpressionValue::ExpressionValue(shared_ptr<Location> location):
Expression(ExpressionKind::VALUE, nullptr, location) { }

ExpressionValueKind ExpressionValue::getValueKind() {
    return valueKind;
}

string ExpressionValue::getIdentifier() {
    return identifier;
}

shared_ptr<Expression> ExpressionValue::getIndexExpression() {
    return indexExpression;
}