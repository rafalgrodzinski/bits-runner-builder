#include "ExpressionVariable.h"

ExpressionVariable::ExpressionVariable(string name, shared_ptr<Expression> indexExpression):
Expression(ExpressionKind::VAR, nullptr), name(name), indexExpression(indexExpression) { }

string ExpressionVariable::getName() {
    return name;
}

shared_ptr<Expression> ExpressionVariable::getIndexExpression() {
    return indexExpression;
}
