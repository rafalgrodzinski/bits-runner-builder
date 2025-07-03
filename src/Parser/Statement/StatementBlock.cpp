#include "StatementBlock.h"

#include "Parser/Statement/StatementExpression.h"

StatementBlock::StatementBlock(vector<shared_ptr<Statement>> statements):
Statement(StatementKind::BLOCK), statements(statements) { }

vector<shared_ptr<Statement>> StatementBlock::getStatements() {
    return statements;
}
