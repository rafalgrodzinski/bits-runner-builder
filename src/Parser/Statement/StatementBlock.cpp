#include "StatementBlock.h"

StatementBlock::StatementBlock(const vector<shared_ptr<Statement>> &statements, shared_ptr<Location> location):
Statement(StatementKind::BLOCK, location), statements(statements) { }

vector<shared_ptr<Statement>> StatementBlock::getStatements() const {
    return statements;
}
