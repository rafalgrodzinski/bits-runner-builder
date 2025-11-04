#include "ExpressionGrouping.h"

ExpressionGrouping::ExpressionGrouping(shared_ptr<Expression> subExpression):
Expression(ExpressionKind::GROUPING, subExpression->getValueType()), subExpression(subExpression) { }

shared_ptr<Expression> ExpressionGrouping::getSubExpression() {
    return subExpression;
}
