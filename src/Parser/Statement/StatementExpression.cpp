#include "StatementExpression.h"

#include "Parser/Expression/Expression.h"

StatementExpression::StatementExpression(shared_ptr<Expression> expression):
Statement(StatementKind::EXPRESSION), expression(expression) { }

shared_ptr<Expression> StatementExpression::getExpression() {
    return expression;
}
