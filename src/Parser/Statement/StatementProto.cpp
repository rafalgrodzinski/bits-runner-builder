#include "StatementProto.h"

StatementProto::StatementProto(
    bool shouldExport,
    string name,
    vector<shared_ptr<StatementVariable>> variableStatements,
    vector<shared_ptr<StatementFunctionDeclaration>> functionDeclarationStatements,
    shared_ptr<Location> location
):
Statement(StatementKind::PROTO, location), shouldExport(shouldExport), name(std::move(name)), variableStatements(std::move(variableStatements)), functionDeclarationStatements(std::move(functionDeclarationStatements)) { }

bool StatementProto::getShouldExport() const {
    return shouldExport;
}

string StatementProto::getName() const {
    return name;
}

vector<shared_ptr<StatementVariable>> StatementProto::getVariableStatements() const {
    return variableStatements;
}

vector<shared_ptr<StatementFunctionDeclaration>> StatementProto::getFunctionDeclarationStatements() const {
    return functionDeclarationStatements;
}