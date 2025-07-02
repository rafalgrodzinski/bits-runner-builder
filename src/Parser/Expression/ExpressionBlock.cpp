#include "ExpressionBlock.h"

#include "Parser/Expression/ExpressionLiteral.h"
#include "Parser/Statement/StatementExpression.h"
#include "Parser/Statement/StatementBlock.h"

ExpressionBlock::ExpressionBlock(vector<shared_ptr<Statement>> statements):
Expression(ExpressionKind::BLOCK, ValueType::NONE) {
    if (!statements.empty() && statements.back()->getKind() == StatementKind::EXPRESSION) {
        resultStatementExpression = dynamic_pointer_cast<StatementExpression>(statements.back());
        valueType = resultStatementExpression->getExpression()->getValueType();
        statements.pop_back();
    } else {
        resultStatementExpression = make_shared<StatementExpression>(make_shared<ExpressionLiteral>());
    }
    statementBlock = make_shared<StatementBlock>(statements);
}

shared_ptr<StatementBlock> ExpressionBlock::getStatementBlock() {
    return statementBlock;
}

shared_ptr<StatementExpression> ExpressionBlock::getResultStatementExpression() {
    return resultStatementExpression;
}

string ExpressionBlock::toString(int indent) {
    return "";
}