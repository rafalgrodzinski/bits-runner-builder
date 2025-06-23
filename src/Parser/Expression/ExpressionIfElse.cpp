#include "ExpressionIfElse.h"

#include "Parser/Expression/ExpressionBlock.h"

ExpressionIfElse::ExpressionIfElse(shared_ptr<Expression> condition, shared_ptr<ExpressionBlock> thenBlock, shared_ptr<ExpressionBlock> elseBlock):
Expression(ExpressionKind::IF_ELSE, ValueType::NONE), condition(condition), thenBlock(thenBlock), elseBlock(elseBlock) { }

shared_ptr<Expression> ExpressionIfElse::getCondition() {
    return condition;
}

shared_ptr<ExpressionBlock> ExpressionIfElse::getThenBlock() {
    return thenBlock;
}

shared_ptr<ExpressionBlock> ExpressionIfElse::getElseBlock() {
    return elseBlock;
}

string ExpressionIfElse::toString(int indent) {
    string value;
    value += "IF(" + condition->toString(0) + "):\n";

    value += thenBlock->toString(indent+1);
    if (elseBlock != nullptr) {
        for (int ind=0; ind<indent; ind++)
            value += "  ";
        value += "ELSE:\n";
        value += elseBlock->toString(indent+1);
    }
    for (int ind=0; ind<indent; ind++)
        value += "  ";
    value += ";";

    return  value;
}
