#include "Parser/Statement/StatementReturn.h"

#include "Parser/Expression/Expression.h"

StatementReturn::StatementReturn(shared_ptr<Expression> expression):
Statement(StatementKind::RETURN), expression(expression) { }

shared_ptr<Expression> StatementReturn::getExpression() {
    return expression;
}
