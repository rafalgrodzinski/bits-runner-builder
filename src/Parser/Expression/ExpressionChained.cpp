#include "ExpressionChained.h"

ExpressionChained::ExpressionChained(vector<shared_ptr<Expression>> chainExpressions):
Expression(ExpressionKind::CHAINED, nullptr), chainExpressions(chainExpressions) { }

vector<shared_ptr<Expression>> ExpressionChained::getChainExpressions() {
    return chainExpressions;
}