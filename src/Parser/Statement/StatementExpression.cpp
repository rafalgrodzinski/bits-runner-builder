#include "StatementExpression.h"

#include "Parser/Expression/Expression.h"

StatementExpression::StatementExpression(shared_ptr<Expression> expression, int line, int column):
Statement(StatementKind::EXPRESSION, line, column), expression(expression) { }

shared_ptr<Expression> StatementExpression::getExpression() {
    return expression;
}
