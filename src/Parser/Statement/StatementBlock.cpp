#include "StatementBlock.h"

#include "Parser/Statement/StatementExpression.h"

StatementBlock::StatementBlock(vector<shared_ptr<Statement>> statements, int line, int column):
Statement(StatementKind::BLOCK, line, column), statements(statements) { }

vector<shared_ptr<Statement>> StatementBlock::getStatements() {
    return statements;
}
