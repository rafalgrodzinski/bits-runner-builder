#include "ExpressionChained.h"

ExpressionChained::ExpressionChained(shared_ptr<ExpressionChained> parentExpression, shared_ptr<Expression> expression):
Expression(ExpressionKind::CHAINED, nullptr), parentExpression(parentExpression), expression(expression) { }

shared_ptr<ExpressionChained> ExpressionChained::getParentExpression() {
    return parentExpression;
}

shared_ptr<Expression> ExpressionChained::getExpression() {
    return expression;
}
