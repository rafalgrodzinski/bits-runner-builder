#include "StatementVariableDeclaration.h"

StatementVariableDeclaration::StatementVariableDeclaration(bool shouldExport, string identifier, shared_ptr<ValueType> valueType, int line, int column):
Statement(StatementKind::VARIABLE_DECLARATION, line, column), shouldExport(shouldExport), identifier(identifier), valueType(valueType) { }

bool StatementVariableDeclaration::getShouldExport() {
    return shouldExport;
}

string StatementVariableDeclaration::getIdentifier() {
    return identifier;
}

shared_ptr<ValueType> StatementVariableDeclaration::getValueType() {
    return valueType;
}