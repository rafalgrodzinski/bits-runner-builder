#include "StatementRawFunction.h"

#include "Parser/ValueType.h"

StatementRawFunction::StatementRawFunction(
    string name,
    string constraints,
    vector<pair<string, shared_ptr<ValueType>>> arguments,
    shared_ptr<ValueType> returnValueType,
    string rawSource,
    shared_ptr<Location> location
):
Statement(StatementKind::RAW_FUNCTION, location), name(name), constraints(constraints), arguments(arguments), returnValueType(returnValueType), rawSource(rawSource) { }

string StatementRawFunction::getName() {
    return name;
}

string StatementRawFunction::getConstraints() {
    return constraints;
}

vector<pair<string, shared_ptr<ValueType>>> StatementRawFunction::getArguments() {
    return arguments;
}

shared_ptr<ValueType> StatementRawFunction::getReturnValueType() {
    return returnValueType;
}

shared_ptr<ValueType> StatementRawFunction::getValueType() {
    vector<shared_ptr<ValueType>> argumentTypes;
    for (auto &argument : arguments)
        argumentTypes.push_back(argument.second);

    return ValueType::fun(argumentTypes, returnValueType);
}

string StatementRawFunction::getRawSource() {
    return rawSource;
}