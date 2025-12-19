#include "StatementExpression.h"

#include "Parser/Expression/Expression.h"

StatementExpression::StatementExpression(shared_ptr<Expression> expression, shared_ptr<Location> location):
Statement(StatementKind::EXPRESSION, location), expression(expression) { }

shared_ptr<Expression> StatementExpression::getExpression() {
    return expression;
}
