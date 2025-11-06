#include "StatementFunctionDeclaration.h"

#include "Parser/Statement/StatementReturn.h"

StatementFunctionDeclaration::StatementFunctionDeclaration(
    bool shouldExport,
    string name,
    vector<pair<string, shared_ptr<ValueType>>> arguments,
    shared_ptr<ValueType> returnValueType,
    int line,
    int column
):
Statement(StatementKind::FUNCTION_DECLARATION, line, column), shouldExport(shouldExport), name(name), arguments(arguments), returnValueType(returnValueType) { }

bool StatementFunctionDeclaration::getShouldExport() {
    return shouldExport;
}

string StatementFunctionDeclaration::getName() {
    return name;
}

vector<pair<string, shared_ptr<ValueType>>> StatementFunctionDeclaration::getArguments() {
    return arguments;
}

shared_ptr<ValueType> StatementFunctionDeclaration::getReturnValueType() {
    return returnValueType;
}
