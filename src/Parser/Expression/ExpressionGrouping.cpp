#include "ExpressionGrouping.h"

ExpressionGrouping::ExpressionGrouping(shared_ptr<Expression> subExpression, int line, int column):
Expression(ExpressionKind::GROUPING, nullptr, line, column), subExpression(subExpression) { }

shared_ptr<Expression> ExpressionGrouping::getSubExpression() {
    return subExpression;
}
