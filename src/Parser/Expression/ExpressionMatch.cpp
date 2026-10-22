#include "ExpressionMatch.h"

#include "Parser/Pattern.h"

using namespace std;

// MARK: - Public

ExpressionMatch::ExpressionMatch(
    shared_ptr<Expression> switchExpression,
    std::vector<std::pair<std::shared_ptr<Pattern>, std::shared_ptr<Expression>>> casePairs,
    shared_ptr<Expression> elseExpression,
    shared_ptr<Location> location
):
Expression(ExpressionKind::MATCH, nullptr, location),
switchExpression(switchExpression),
casePairs(casePairs),
elseExpression(elseExpression) { }

std::shared_ptr<Expression> ExpressionMatch::getSwitchExpression() {
    return switchExpression;
}

std::vector<std::pair<std::shared_ptr<Pattern>, std::shared_ptr<Expression>>> ExpressionMatch::getCasePairs() {
    return casePairs;
}

std::shared_ptr<Expression> ExpressionMatch::getElseExpression() {
    return elseExpression;
}