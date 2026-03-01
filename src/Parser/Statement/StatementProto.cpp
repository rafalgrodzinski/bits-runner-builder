#include "StatementProto.h"

StatementProto::StatementProto(
    bool shouldExport,
    string name,
    vector<shared_ptr<StatementVariable>> variableStatements,
    vector<shared_ptr<StatementFunctionDeclaration>> functionDeclarationStatements,
    shared_ptr<Location> location
) :
Statement(StatementKind::PROTO, location), shouldExport(shouldExport), name(name), variableStatements(variableStatements), functionDeclarationStatements(functionDeclarationStatements) { }

bool StatementProto::getShouldExport() {
    return shouldExport;
}

string StatementProto::getName() {
    return name;
}

vector<shared_ptr<StatementVariable>> StatementProto::getVariableStatements() {
    return variableStatements;
}

vector<shared_ptr<StatementFunctionDeclaration>> StatementProto::getFunctionDeclarationStatements() {
    return functionDeclarationStatements;
}