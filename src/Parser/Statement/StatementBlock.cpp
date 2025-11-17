#include "StatementBlock.h"

StatementBlock::StatementBlock(vector<shared_ptr<Statement>> statements, int line, int column):
Statement(StatementKind::BLOCK, line, column), statements(statements) { }

vector<shared_ptr<Statement>> StatementBlock::getStatements() {
    return statements;
}
