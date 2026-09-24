#include "ExpressionChained.h"

using namespace std;

// MARK: - Public

ExpressionChained::ExpressionChained(const vector<shared_ptr<Expression>> &chainExpressions, shared_ptr<Location> location):
Expression(ExpressionKind::CHAINED, nullptr, location), chainExpressions(chainExpressions) { }

vector<shared_ptr<Expression>> ExpressionChained::getChainExpressions() const {
    return chainExpressions;
}