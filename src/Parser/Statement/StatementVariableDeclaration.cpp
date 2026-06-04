#include "StatementVariableDeclaration.h"

StatementVariableDeclaration::StatementVariableDeclaration(bool shouldExport, const string &identifier, shared_ptr<ValueType> valueType, shared_ptr<Location> location):
Statement(StatementKind::VARIABLE_DECLARATION, location), shouldExport(shouldExport), identifier(std::move(identifier)), valueType(valueType) { }

bool StatementVariableDeclaration::getShouldExport() const {
    return shouldExport;
}

string StatementVariableDeclaration::getIdentifier() const {
    return identifier;
}

shared_ptr<ValueType> StatementVariableDeclaration::getValueType() const {
    return valueType;
}