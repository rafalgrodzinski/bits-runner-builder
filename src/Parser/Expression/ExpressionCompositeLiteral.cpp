#include "ExpressionCompositeLiteral.h"

shared_ptr<ExpressionCompositeLiteral> ExpressionCompositeLiteral::expressionCompositeLiteralForExpressions(vector<shared_ptr<Expression>> expressions) {
    shared_ptr<ExpressionCompositeLiteral> expression = make_shared<ExpressionCompositeLiteral>();
    expression->expressions = expressions;
    return expression;
}

ExpressionCompositeLiteral::ExpressionCompositeLiteral():
Expression(ExpressionKind::COMPOSITE_LITERAL, nullptr) { }

vector<shared_ptr<Expression>> ExpressionCompositeLiteral::getExpressions() {
    return expressions;
} 