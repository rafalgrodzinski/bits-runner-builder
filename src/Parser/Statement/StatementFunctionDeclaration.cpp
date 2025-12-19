#include "StatementFunctionDeclaration.h"

#include "Parser/Statement/StatementReturn.h"
#include "Parser/ValueType.h"

StatementFunctionDeclaration::StatementFunctionDeclaration(
    bool shouldExport,
    string name,
    vector<pair<string, shared_ptr<ValueType>>> arguments,
    shared_ptr<ValueType> returnValueType,
    shared_ptr<Location> location
):
Statement(StatementKind::FUNCTION_DECLARATION, location), shouldExport(shouldExport), name(name), arguments(arguments), returnValueType(returnValueType) { }

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

shared_ptr<ValueType> StatementFunctionDeclaration::getValueType() {
    vector<shared_ptr<ValueType>> argumentTypes;
    for (auto &argument : arguments)
        argumentTypes.push_back(argument.second);

    return ValueType::fun(argumentTypes, returnValueType);
}
