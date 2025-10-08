#include "StatementVariableDeclaration.h"

StatementVariableDeclaration::StatementVariableDeclaration(bool shouldExport, string name, shared_ptr<ValueType> valueType):
Statement(StatementKind::VARIABLE_DECLARATION), shouldExport(shouldExport), name(name), valueType(valueType) { }

bool StatementVariableDeclaration::getShouldExport() {
    return shouldExport;
}

string StatementVariableDeclaration::getName() {
    return name;
}

shared_ptr<ValueType> StatementVariableDeclaration::getValueType() {
    return valueType;
}