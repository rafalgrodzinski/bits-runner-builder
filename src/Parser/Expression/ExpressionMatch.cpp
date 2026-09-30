#include "ExpressionMatch.h"

using namespace std;

// MARK: - Public

ExpressionMatch::ExpressionMatch(shared_ptr<Expression> switchExpression, shared_ptr<Location> location):
Expression(ExpressionKind::MATCH, nullptr, location),
switchExpression(switchExpression) { }

std::shared_ptr<Expression> ExpressionMatch::getSwitchExpression() {
    return switchExpression;
}