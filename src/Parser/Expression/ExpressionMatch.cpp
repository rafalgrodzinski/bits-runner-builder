#include "ExpressionMatch.h"

using namespace std;

// MARK: - Public

ExpressionMatch::ExpressionMatch(
    shared_ptr<Expression> switchExpression,
    shared_ptr<Expression> elseExpression,
    shared_ptr<Location> location
):
Expression(ExpressionKind::MATCH, nullptr, location),
switchExpression(switchExpression),
elseExpression(elseExpression) { }

std::shared_ptr<Expression> ExpressionMatch::getSwitchExpression() {
    return switchExpression;
}

std::shared_ptr<Expression> ExpressionMatch::getElseExpression() {
    return elseExpression;
}