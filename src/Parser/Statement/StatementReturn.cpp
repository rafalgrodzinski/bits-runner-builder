#include "Parser/Statement/StatementReturn.h"

#include "Parser/Expression/Expression.h"

using namespace std;

// MARK: - Public

StatementReturn::StatementReturn(shared_ptr<Expression> expression, shared_ptr<Location> location):
Statement(StatementKind::RETURN, location) {
    this->expression = expression ? expression : Expression::none(location);
 }

shared_ptr<Expression> StatementReturn::getExpression() const {
    return expression;
}
