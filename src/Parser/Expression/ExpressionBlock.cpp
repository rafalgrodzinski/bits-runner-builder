#include "ExpressionBlock.h"

#include "Parser/Expression/ExpressionLiteral.h"
#include "Parser/Statement/StatementExpression.h"

ExpressionBlock::ExpressionBlock(vector<shared_ptr<Statement>> statements):
Expression(ExpressionKind::BLOCK, ValueType::NONE), statements(statements) {
    if (!statements.empty() && statements.back()->getKind() == StatementKind::EXPRESSION) {
        resultExpression = dynamic_pointer_cast<StatementExpression>(statements.back());
        this->statements.pop_back();
        valueType = resultExpression->getExpression()->getValueType();
    } else {
        resultExpression = make_shared<StatementExpression>(ExpressionLiteral::none);
    }
}

string ExpressionBlock::toString(int indent) {
    return "";
}