#include "ExpressionChained.h"

ExpressionChained::ExpressionChained(vector<shared_ptr<Expression>> chainExpressions, shared_ptr<Location> location):
Expression(ExpressionKind::CHAINED, nullptr, location), chainExpressions(chainExpressions) { }

vector<shared_ptr<Expression>> ExpressionChained::getChainExpressions() {
    return chainExpressions;
}