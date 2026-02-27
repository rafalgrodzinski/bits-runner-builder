#include "StatementProto.h"

StatementProto::StatementProto(
    bool shouldExport,
    string name,
    vector<shared_ptr<StatementVariable>> variableStatements,
    vector<shared_ptr<StatementFunction>> functionStatements,
    shared_ptr<Location> location
) :
Statement(StatementKind::BLOB, location), shouldExport(shouldExport), name(name), variableStatements(variableStatements), functionStatements(functionStatements) { }

bool StatementProto::getShouldExport() {
    return shouldExport;
}

string StatementProto::getName() {
    return name;
}

vector<shared_ptr<StatementVariable>> StatementProto::getVariableStatements() {
    return variableStatements;
}

vector<shared_ptr<StatementFunction>> StatementProto::getFunctionStatements() {
    return functionStatements;
}