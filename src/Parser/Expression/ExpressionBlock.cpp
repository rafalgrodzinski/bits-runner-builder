#include "ExpressionBlock.h"

#include "Parser/Expression/ExpressionLiteral.h"
#include "Parser/Statement/StatementExpression.h"
#include "Parser/Statement/StatementBlock.h"

#include "Lexer/Location.h"

ExpressionBlock::ExpressionBlock(vector<shared_ptr<Statement>> statements, int line, int column):
Expression(ExpressionKind::BLOCK, nullptr, line, column) {
    if (!statements.empty() && statements.back()->getKind() == StatementKind::EXPRESSION) {
        resultStatementExpression = dynamic_pointer_cast<StatementExpression>(statements.back());
        statements.pop_back();
    } else {
        resultStatementExpression = make_shared<StatementExpression>(Expression::NONE, make_shared<Location>("", line, column));
    }
    statementBlock = make_shared<StatementBlock>(statements, make_shared<Location>("", line, column));
}

shared_ptr<StatementBlock> ExpressionBlock::getStatementBlock() {
    return statementBlock;
}

shared_ptr<StatementExpression> ExpressionBlock::getResultStatementExpression() {
    return resultStatementExpression;
}
