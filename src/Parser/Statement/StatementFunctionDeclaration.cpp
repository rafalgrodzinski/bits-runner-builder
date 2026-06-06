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
Statement(StatementKind::FUNCTION_DECLARATION, location), shouldExport(shouldExport), name(std::move(name)), arguments(std::move(arguments)), returnValueType(returnValueType) { }

bool StatementFunctionDeclaration::getShouldExport() const {
    return shouldExport;
}

string StatementFunctionDeclaration::getName() const {
    return name;
}

vector<pair<string, shared_ptr<ValueType>>> StatementFunctionDeclaration::getArguments() const {
    return arguments;
}

shared_ptr<ValueType> StatementFunctionDeclaration::getReturnValueType() const {
    return returnValueType;
}

shared_ptr<ValueType> StatementFunctionDeclaration::getValueType() const {
    vector<shared_ptr<ValueType>> argumentTypes;
    for (auto &argument : arguments)
        argumentTypes.push_back(argument.second);

    return ValueType::fun(argumentTypes, returnValueType);
}
