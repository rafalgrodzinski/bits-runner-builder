#include "StatementFunctionDeclaration.h"

#include "Parser/Statement/StatementReturn.h"

StatementFunctionDeclaration::StatementFunctionDeclaration(string name, vector<pair<string, shared_ptr<ValueType>>> arguments, shared_ptr<ValueType> returnValueType):
Statement(StatementKind::FUNCTION_DECLARATION), name(name), arguments(arguments), returnValueType(returnValueType) { }

string StatementFunctionDeclaration::getName() {
    return name;
}

vector<pair<string, shared_ptr<ValueType>>> StatementFunctionDeclaration::getArguments() {
    return arguments;
}

shared_ptr<ValueType> StatementFunctionDeclaration::getReturnValueType() {
    return returnValueType;
}
