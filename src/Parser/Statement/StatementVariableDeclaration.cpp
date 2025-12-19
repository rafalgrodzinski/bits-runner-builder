#include "StatementVariableDeclaration.h"

StatementVariableDeclaration::StatementVariableDeclaration(bool shouldExport, string identifier, shared_ptr<ValueType> valueType, shared_ptr<Location> location):
Statement(StatementKind::VARIABLE_DECLARATION, location), shouldExport(shouldExport), identifier(identifier), valueType(valueType) { }

bool StatementVariableDeclaration::getShouldExport() {
    return shouldExport;
}

string StatementVariableDeclaration::getIdentifier() {
    return identifier;
}

shared_ptr<ValueType> StatementVariableDeclaration::getValueType() {
    return valueType;
}