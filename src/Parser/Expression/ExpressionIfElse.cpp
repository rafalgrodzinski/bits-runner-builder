#include "ExpressionIfElse.h"

#include "Parser/Expression/ExpressionBlock.h"

ExpressionIfElse::ExpressionIfElse(shared_ptr<Expression> condition, shared_ptr<ExpressionBlock> thenBlock, shared_ptr<Expression> elseExpression):
Expression(ExpressionKind::IF_ELSE, nullptr), condition(condition), thenBlock(thenBlock), elseExpression(elseExpression) {
    // Figure out resulting type
    if (elseExpression == nullptr || thenBlock->getValueType() == elseExpression->getValueType())
        valueType = thenBlock->getValueType();
}

shared_ptr<Expression> ExpressionIfElse::getCondition() {
    return condition;
}

shared_ptr<ExpressionBlock> ExpressionIfElse::getThenBlock() {
    return thenBlock;
}

shared_ptr<Expression> ExpressionIfElse::getElseExpression() {
    return elseExpression;
}
