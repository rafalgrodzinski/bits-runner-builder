#include "StatementFunctionDeclaration.h"

#include "Parser/Statement/StatementReturn.h"
#include "Parser/ValueType.h"

StatementFunctionDeclaration::StatementFunctionDeclaration(
    bool shouldExport,
    const string &name,
    const string &moduleName,
    const vector<pair<string, shared_ptr<ValueType>>> &arguments,
    shared_ptr<ValueType> returnValueType,
    shared_ptr<Location> location
):
Statement(StatementKind::FUNCTION_DECLARATION, location), shouldExport(shouldExport), name(name), moduleName(moduleName), arguments(arguments), returnValueType(returnValueType) { }

bool StatementFunctionDeclaration::getShouldExport() const {
    return shouldExport;
}

string StatementFunctionDeclaration::getName() const {
    return name;
}

string StatementFunctionDeclaration::getGlobalName() const {
    return format("{}.{}", moduleName, name); 
}

string StatementFunctionDeclaration::getModuleName() const {
    return moduleName;
}

void StatementFunctionDeclaration::setModuleName(const string &moduleName) {
    if (!this->moduleName.empty())
        return;

    this->moduleName = moduleName;

    // arguments
    for (const pair<string, shared_ptr<ValueType>> &argumentPair : arguments)
        argumentPair.second->setModuleName(moduleName);

    // return
    
    returnValueType->setModuleName(moduleName);
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
