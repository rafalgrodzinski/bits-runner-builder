#include "ExpressionChained.h"

ExpressionChained::ExpressionChained(const vector<shared_ptr<Expression>> &chainExpressions, shared_ptr<Location> location):
Expression(ExpressionKind::CHAINED, nullptr, location), chainExpressions(std::move(chainExpressions)) { }

vector<shared_ptr<Expression>> ExpressionChained::getChainExpressions() const {
    return chainExpressions;
}