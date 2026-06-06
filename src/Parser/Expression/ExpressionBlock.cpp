#include "ExpressionBlock.h"

#include "Parser/Expression/ExpressionLiteral.h"
#include "Parser/Statement/StatementExpression.h"
#include "Parser/Statement/StatementBlock.h"

#include "Lexer/Location.h"

ExpressionBlock::ExpressionBlock(vector<shared_ptr<Statement>> statements, shared_ptr<Location> location):
Expression(ExpressionKind::BLOCK, nullptr, location) {
    vector<shared_ptr<Statement>> blockStatements = std::move(statements);
    if (!blockStatements.empty() && blockStatements.back()->getKind() == StatementKind::EXPRESSION) {
        resultStatementExpression = dynamic_pointer_cast<StatementExpression>(blockStatements.back());
        blockStatements.pop_back();
    } else {
        resultStatementExpression = make_shared<StatementExpression>(Expression::NONE, location);
    }
    statementBlock = make_shared<StatementBlock>(blockStatements, location);
}

shared_ptr<StatementBlock> ExpressionBlock::getStatementBlock() const {
    return statementBlock;
}

shared_ptr<StatementExpression> ExpressionBlock::getResultStatementExpression() const {
    return resultStatementExpression;
}
