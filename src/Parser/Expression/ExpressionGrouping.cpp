#include "ExpressionGrouping.h"

ExpressionGrouping::ExpressionGrouping(shared_ptr<Expression> expression):
Expression(ExpressionKind::GROUPING, expression->getValueType()), expression(expression) { }

shared_ptr<Expression> ExpressionGrouping::getExpression() {
    return expression;
}

string ExpressionGrouping::toString(int indent) {
    return "( " + expression->toString(0) + " )";
}
