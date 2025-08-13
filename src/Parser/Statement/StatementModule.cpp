#include "StatementModule.h"

StatementModule::StatementModule(string name, vector<shared_ptr<Statement>> statements):
Statement(StatementKind::MODULE), name(name), statements(statements) { }

string StatementModule::getName() {
    return name;
}

vector<shared_ptr<Statement>> StatementModule::getStatements() {
    return statements;
}