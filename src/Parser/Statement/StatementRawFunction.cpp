#include "StatementRawFunction.h"

#include "Parser/ValueType.h"

StatementRawFunction::StatementRawFunction(
    bool shouldExport,
    string name,
    string constraints,
    vector<pair<string, shared_ptr<ValueType>>> arguments,
    shared_ptr<ValueType> returnValueType,
    string rawSource,
    shared_ptr<Location> location
):
Statement(StatementKind::RAW_FUNCTION, location), shouldExport(shouldExport), name(std::move(name)), constraints(std::move(constraints)), arguments(std::move(arguments)), returnValueType(returnValueType), rawSource(std::move(rawSource)) { }

bool StatementRawFunction::getShouldExport() const {
    return shouldExport;
}

string StatementRawFunction::getName() const {
    return name;
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