#include "StatementProto.h"

StatementProto::StatementProto(
    bool shouldExport,
    const string &name,
    const vector<shared_ptr<StatementVariable>> &variableStatements,
    const vector<shared_ptr<StatementFunctionDeclaration>> &functionDeclarationStatements,
    shared_ptr<Location> location
):
Statement(StatementKind::PROTO, location),
shouldExport(shouldExport),
name(name),
variableStatements(variableStatements),
functionDeclarationStatements(functionDeclarationStatements) { }

bool StatementProto::getShouldExport() const {
    return shouldExport;
}

string StatementProto::getName() const {
    return name;
}

string StatementProto::getGlobalName() const {
    string moduleName = this->moduleName;
    if (moduleName.empty())
        moduleName = "{UNDEFINED}";

    return format("{}.{}", moduleName, name);
}

string StatementProto::getModuleName() const {
    return moduleName;
}

void StatementProto::setModuleName(const string &moduleName) {
    if (this->moduleName.empty())
        this->moduleName = moduleName;
}

vector<shared_ptr<StatementVariable>> StatementProto::getVariableStatements() const {
    return variableStatements;
}

vector<shared_ptr<StatementFunctionDeclaration>> StatementProto::getFunctionDeclarationStatements() const {
    return functionDeclarationStatements;
}

shared_ptr<StatementProtoDeclaration> StatementProto::getDeclaration() const {
    return make_shared<StatementProtoDeclaration>(
        shouldExport,
        name,
        moduleName,
        getLocation()
    );
}