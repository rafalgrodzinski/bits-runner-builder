#include "StatementModule.h"

StatementModule::StatementModule(string name, vector<shared_ptr<Statement>> statements, vector<shared_ptr<Statement>> headerStatements):
Statement(StatementKind::MODULE), name(name), statements(statements), headerStatements(headerStatements) { }

string StatementModule::getName() {
    return name;
}

vector<shared_ptr<Statement>> StatementModule::getStatements() {
    return statements;
}

vector<shared_ptr<Statement>> StatementModule::getHeaderStatements() {
    return headerStatements;
}