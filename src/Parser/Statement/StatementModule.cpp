#include "StatementModule.h"

StatementModule::StatementModule(
    string name,
    vector<shared_ptr<Statement>> statements,
    vector<shared_ptr<Statement>> headerStatements,
    vector<shared_ptr<Statement>> exportedHeaderStatements,
    int line,
    int column
):
Statement(StatementKind::MODULE, line, column), name(name), statements(statements), headerStatements(headerStatements), exportedHeaderStatements(exportedHeaderStatements) { }

string StatementModule::getName() {
    return name;
}

vector<shared_ptr<Statement>> StatementModule::getStatements() {
    return statements;
}

vector<shared_ptr<Statement>> StatementModule::getHeaderStatements() {
    return headerStatements;
}

vector<shared_ptr<Statement>> StatementModule::getExportedHeaderStatements() {
    return exportedHeaderStatements;
}