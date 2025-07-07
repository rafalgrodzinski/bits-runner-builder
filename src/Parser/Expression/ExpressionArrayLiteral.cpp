#include "ExpressionArrayLiteral.h"

ExpressionArrayLiteral::ExpressionArrayLiteral(vector<shared_ptr<Expression>> expressions):
Expression(ExpressionKind::ARRAY_LITERAL, nullptr), expressions(expressions) { }

vector<shared_ptr<Expression>> ExpressionArrayLiteral::getExpressions() {
    return expressions;
}