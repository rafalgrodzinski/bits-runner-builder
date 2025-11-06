#include "ExpressionChained.h"

ExpressionChained::ExpressionChained(vector<shared_ptr<Expression>> chainExpressions, int line, int column):
Expression(ExpressionKind::CHAINED, nullptr, line, column), chainExpressions(chainExpressions) { }

vector<shared_ptr<Expression>> ExpressionChained::getChainExpressions() {
    return chainExpressions;
}