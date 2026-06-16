#include "StatementVariableDeclaration.h"

StatementVariableDeclaration::StatementVariableDeclaration(
    bool shouldExport,
    const string &identifier,
    const string &moduleName,
    shared_ptr<ValueType> valueType,
    shared_ptr<Location> location
):
Statement(StatementKind::VARIABLE_DECLARATION, location), shouldExport(shouldExport), identifier(identifier), moduleName(moduleName), valueType(valueType) { }

bool StatementVariableDeclaration::getShouldExport() const {
    return shouldExport;
}

string StatementVariableDeclaration::getIdentifier() const {
    return identifier;
}

string StatementVariableDeclaration::getGlobalIdentifier() const {
    return format("{}.{}", moduleName, identifier);
}

string StatementVariableDeclaration::getModuleName() const {
    return moduleName;
}

void StatementVariableDeclaration::setModuleName(const string &moduleName) {
    if (this->moduleName.empty())
        this->moduleName = moduleName;
}

shared_ptr<ValueType> StatementVariableDeclaration::getValueType() const {
    return valueType;
}