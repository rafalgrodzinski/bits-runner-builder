#include "Parser/Statement/StatementReturn.h"

#include "Parser/Expression/Expression.h"

StatementReturn::StatementReturn(shared_ptr<Expression> expression, shared_ptr<Location> location):
Statement(StatementKind::RETURN, location) {
    this->expression = expression ? expression : Expression::NONE;
 }

shared_ptr<Expression> StatementReturn::getExpression() {
    return expression;
}
