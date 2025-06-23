#include "StatementBlock.h"

#include "Parser/Statement/StatementExpression.h"

StatementBlock::StatementBlock(vector<shared_ptr<Statement>> statements):
Statement(StatementKind::BLOCK), statements(statements) { }

vector<shared_ptr<Statement>> StatementBlock::getStatements() {
    return statements;
}

string StatementBlock::toString(int indent) {
    string value;
    for (int i=0; i<statements.size(); i++) {
        //for (int ind=0; ind<indent; ind++)
        //    value += "  ";
        value += statements.at(i)->toString(indent);
    }
    for (int ind=0; ind<indent; ind++)
        value += "  ";
    value += "#\n";
    return value;
}