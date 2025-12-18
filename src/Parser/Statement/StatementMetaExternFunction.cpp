#include "Parser/Statement/StatementMetaExternFunction.h"

#include "Parser/ValueType.h"

StatementMetaExternFunction::StatementMetaExternFunction(string name, vector<pair<string, shared_ptr<ValueType>>> arguments, shared_ptr<ValueType> returnValueType, shared_ptr<Location> location):
Statement(StatementKind::META_EXTERN_FUNCTION, location), name(name), arguments(arguments), returnValueType(returnValueType) { }

string StatementMetaExternFunction::getName() {
    return name;
}

vector<pair<string, shared_ptr<ValueType>>> StatementMetaExternFunction::getArguments() {
    return arguments;
}

shared_ptr<ValueType> StatementMetaExternFunction::getReturnValueType() {
    return returnValueType;
}

shared_ptr<ValueType> StatementMetaExternFunction::getValueType() {
    vector<shared_ptr<ValueType>> argumentTypes;
    for (auto &argument : arguments)
        argumentTypes.push_back(argument.second);

    return ValueType::fun(argumentTypes, returnValueType);
}