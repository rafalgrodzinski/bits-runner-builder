#include "Parser/Statement/StatementReturn.h"

#include "Parser/Expression/Expression.h"

StatementReturn::StatementReturn(shared_ptr<Expression> expression, int line, int column):
Statement(StatementKind::RETURN, line, column) {
    this->expression = expression ?: Expression::NONE;
 }

shared_ptr<Expression> StatementReturn::getExpression() {
    return expression;
}
