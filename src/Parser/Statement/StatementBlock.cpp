#include "StatementBlock.h"

#include "Parser/Statement/StatementExpression.h"

StatementBlock::StatementBlock(vector<shared_ptr<Statement>> statements):
Statement(StatementKind::BLOCK), statements(statements) {
    if (!statements.empty() && statements.back()->getKind() == StatementKind::EXPRESSION) {
        statementExpression = dynamic_pointer_cast<StatementExpression>(statements.back());
        this->statements.pop_back();
    }
}

vector<shared_ptr<Statement>> StatementBlock::getStatements() {
    return statements;
}

shared_ptr<StatementExpression> StatementBlock::getStatementExpression() {
    return statementExpression;
}

string StatementBlock::toString(int indent) {
    string value;
    for (int i=0; i<statements.size(); i++) {
        //for (int ind=0; ind<indent; ind++)
        //    value += "  ";
        value += statements.at(i)->toString(indent);
    }
    if (statementExpression != nullptr) {
        for (int ind=0; ind<indent; ind++)
            value += "  ";
        value += "WRAP_UP:\n";
        value += statementExpression->toString(indent);
    }
    for (int ind=0; ind<indent; ind++)
        value += "  ";
    value += "#\n";
    return value;
}