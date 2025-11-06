#include "ExpressionIfElse.h"

#include "Parser/Expression/ExpressionBlock.h"

ExpressionIfElse::ExpressionIfElse(shared_ptr<Expression> conditionExpression, shared_ptr<ExpressionBlock> thenBlockExpression, shared_ptr<Expression> elseExpression, int line, int column):
Expression(ExpressionKind::IF_ELSE, nullptr, line, column), conditionExpression(conditionExpression), thenBlockExpression(thenBlockExpression), elseExpression(elseExpression) {
    // Figure out resulting type
    if (elseExpression == nullptr || thenBlockExpression->getValueType() == elseExpression->getValueType())
        valueType = thenBlockExpression->getValueType();
}

shared_ptr<Expression> ExpressionIfElse::getConditionExpression() {
    return conditionExpression;
}

shared_ptr<ExpressionBlock> ExpressionIfElse::getThenBlockExpression() {
    return thenBlockExpression;
}

shared_ptr<Expression> ExpressionIfElse::getElseExpression() {
    return elseExpression;
}
