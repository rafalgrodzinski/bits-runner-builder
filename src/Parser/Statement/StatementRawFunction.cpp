#include "StatementRawFunction.h"

StatementRawFunction::StatementRawFunction(
    string name,
    string constraints,
    vector<pair<string, shared_ptr<ValueType>>> arguments,
    shared_ptr<ValueType> returnValueType,
    string rawSource,
    int line,
    int column
):
Statement(StatementKind::RAW_FUNCTION, line, column), name(name), constraints(constraints), arguments(arguments), returnValueType(returnValueType), rawSource(rawSource) { }

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

string StatementRawFunction::getRawSource() {
    return rawSource;
}