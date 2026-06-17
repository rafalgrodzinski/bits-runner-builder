#include "StatementRawFunction.h"

#include "Parser/ValueType/ValueType.h"

StatementRawFunction::StatementRawFunction(
    bool shouldExport,
    const string &name,
    const string &constraints,
    const vector<pair<string, shared_ptr<ValueType>>> &arguments,
    shared_ptr<ValueType> returnValueType,
    const string &rawSource,
    shared_ptr<Location> location
):
Statement(StatementKind::RAW_FUNCTION, location),
shouldExport(shouldExport), name(name),
constraints(constraints),
arguments(arguments),
returnValueType(returnValueType),
rawSource(rawSource) { }

bool StatementRawFunction::getShouldExport() const {
    return shouldExport;
}

string StatementRawFunction::getName() const {
    return name;
}

string StatementRawFunction::getGlobalName() const {
    string moduleName = this->moduleName;
    if (moduleName.empty())
        moduleName = "{UNDEFINED}";

    return format("{}.{}", moduleName, name);
}

string StatementRawFunction::getModuleName() const {
    return moduleName;
}

void StatementRawFunction::setModuleName(const string &moduleName) {
    if (this->moduleName.empty())
        this->moduleName = moduleName;
}

string StatementRawFunction::getConstraints() const {
    return constraints;
}

vector<pair<string, shared_ptr<ValueType>>> StatementRawFunction::getArguments() const {
    return arguments;
}

shared_ptr<ValueType> StatementRawFunction::getReturnValueType() const {
    return returnValueType;
}

shared_ptr<ValueType> StatementRawFunction::getValueType() const {
    vector<shared_ptr<ValueType>> argumentTypes;
    for (auto &argument : arguments)
        argumentTypes.push_back(argument.second);

    return ValueType::fun(argumentTypes, returnValueType);
}

string StatementRawFunction::getRawSource() const {
    return rawSource;
}