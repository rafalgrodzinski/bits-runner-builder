#include "ExpressionGrouping.h"

ExpressionGrouping::ExpressionGrouping(shared_ptr<Expression> subExpression, shared_ptr<Location> location):
Expression(ExpressionKind::GROUPING, nullptr, location), subExpression(subExpression) { }

shared_ptr<Expression> ExpressionGrouping::getSubExpression() {
    return subExpression;
}
