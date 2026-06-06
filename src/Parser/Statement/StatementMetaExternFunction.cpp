#include "Parser/Statement/StatementMetaExternFunction.h"

#include "Parser/ValueType.h"

StatementMetaExternFunction::StatementMetaExternFunction(string name, vector<pair<string, shared_ptr<ValueType>>> arguments, shared_ptr<ValueType> returnValueType, shared_ptr<Location> location):
Statement(StatementKind::META_EXTERN_FUNCTION, location), name(std::move(name)), arguments(std::move(arguments)), returnValueType(returnValueType) { }

string StatementMetaExternFunction::getName() const {
    return name;
}

vector<pair<string, shared_ptr<ValueType>>> StatementMetaExternFunction::getArguments() const {
    return arguments;
}

shared_ptr<ValueType> StatementMetaExternFunction::getReturnValueType() const {
    return returnValueType;
}

shared_ptr<ValueType> StatementMetaExternFunction::getValueType() const {
    vector<shared_ptr<ValueType>> argumentTypes;
    for (auto &argument : arguments)
        argumentTypes.push_back(argument.second);

    return ValueType::fun(argumentTypes, returnValueType);
}