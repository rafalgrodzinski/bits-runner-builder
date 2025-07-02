#include "ExpressionIfElse.h"

#include "Parser/Expression/ExpressionBlock.h"

ExpressionIfElse::ExpressionIfElse(shared_ptr<Expression> condition, shared_ptr<ExpressionBlock> thenBlock, shared_ptr<ExpressionBlock> elseBlock):
Expression(ExpressionKind::IF_ELSE, ValueType::NONE), condition(condition), thenBlock(thenBlock), elseBlock(elseBlock) {
    // Figure out resulting type
    if (elseBlock == nullptr || thenBlock->getValueType() == elseBlock->getValueType())
        valueType = thenBlock->getValueType();
}

shared_ptr<Expression> ExpressionIfElse::getCondition() {
    return condition;
}

shared_ptr<ExpressionBlock> ExpressionIfElse::getThenBlock() {
    return thenBlock;
}

shared_ptr<ExpressionBlock> ExpressionIfElse::getElseBlock() {
    return elseBlock;
}
