#include "StatementExchange.h"

using namespace std;

// MARK: - Public

StatementExchange::StatementExchange(
    shared_ptr<Expression> leftExpression,
    shared_ptr<Expression> rightExpression,
    shared_ptr<Location> location
):
Statement(StatementKind::EXCHANGE, location),
leftExpression(leftExpression),
rightExpression(rightExpression) { }

std::shared_ptr<Expression> StatementExchange::getLeftExpression() {
    return leftExpression;
}

std::shared_ptr<Expression> StatementExchange::getRightExpression() {
    return rightExpression;
}