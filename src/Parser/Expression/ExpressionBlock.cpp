#include "ExpressionBlock.h"

#include "Parser/Expression/ExpressionLiteral.h"
#include "Parser/Statement/StatementExpression.h"
#include "Parser/Statement/StatementBlock.h"

#include "Lexer/Location.h"

ExpressionBlock::ExpressionBlock(vector<shared_ptr<Statement>> statements, shared_ptr<Location> location):
Expression(ExpressionKind::BLOCK, nullptr, location) {
    if (!statements.empty() && statements.back()->getKind() == StatementKind::EXPRESSION) {
        resultStatementExpression = dynamic_pointer_cast<StatementExpression>(statements.back());
        statements.pop_back();
    } else {
        resultStatementExpression = make_shared<StatementExpression>(Expression::NONE, location);
    }
    statementBlock = make_shared<StatementBlock>(statements, location);
}

shared_ptr<StatementBlock> ExpressionBlock::getStatementBlock() {
    return statementBlock;
}

shared_ptr<StatementExpression> ExpressionBlock::getResultStatementExpression() {
    return resultStatementExpression;
}
