#include "ExpressionIfElse.h"

ExpressionIfElse::ExpressionIfElse(shared_ptr<Expression> conditionExpression, shared_ptr<Expression> thenExpression, shared_ptr<Expression> elseExpression, shared_ptr<Location> location):
Expression(ExpressionKind::IF_ELSE, nullptr, location), conditionExpression(conditionExpression), thenExpression(thenExpression), elseExpression(elseExpression) { }

shared_ptr<Expression> ExpressionIfElse::getConditionExpression() const {
    return conditionExpression;
}

shared_ptr<Expression> ExpressionIfElse::getThenExpression() const {
    return thenExpression;
}

shared_ptr<Expression> ExpressionIfElse::getElseExpression() const {
    return elseExpression;
}
