#include "ExpressionIfElse.h"

ExpressionIfElse::ExpressionIfElse(shared_ptr<Expression> conditionExpression, shared_ptr<Expression> thenExpression, shared_ptr<Expression> elseExpression, int line, int column):
Expression(ExpressionKind::IF_ELSE, nullptr, line, column), conditionExpression(conditionExpression), thenExpression(thenExpression), elseExpression(elseExpression) { }

shared_ptr<Expression> ExpressionIfElse::getConditionExpression() {
    return conditionExpression;
}

shared_ptr<Expression> ExpressionIfElse::getThenExpression() {
    return thenExpression;
}

shared_ptr<Expression> ExpressionIfElse::getElseExpression() {
    return elseExpression;
}
