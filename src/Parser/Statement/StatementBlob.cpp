#include "StatementBlob.h"

#include "Parser/ValueType.h"

StatementBlob::StatementBlob(
    bool shouldExport,
    string name,
    vector<shared_ptr<Statement>> variableStatements,
    vector<shared_ptr<Statement>> functionStatements,
    shared_ptr<Location> location
):
Statement(StatementKind::BLOB, location), shouldExport(shouldExport), name(name), variableStatements(variableStatements), functionStatements(functionStatements) { }

bool StatementBlob::getShouldExport() {
    return shouldExport;
}

string StatementBlob::getName() {
    return name;
}

vector<shared_ptr<Statement>> StatementBlob::getVariableStatements() {
    return variableStatements;
}

vector<shared_ptr<Statement>> StatementBlob::getFunctionStatements() {
    return functionStatements;
}