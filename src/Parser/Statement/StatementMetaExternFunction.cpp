#include "Parser/Statement/StatementMetaExternFunction.h"

#include "Parser/ValueType.h"

StatementMetaExternFunction::StatementMetaExternFunction(
    const string &name,
    const vector<pair<string, shared_ptr<ValueType>>> &arguments,
    shared_ptr<ValueType> returnValueType,
    shared_ptr<Location> location
):
Statement(StatementKind::META_EXTERN_FUNCTION, location), arguments(arguments), returnValueType(returnValueType) {
    symbolName = name;

    size_t pos = name.find('.');
    if (pos != string::npos) {
        this->moduleName = name.substr(0, pos);
        this->name = name.substr(pos + 1, name.size());
    } else {
        this->name = name;
    }
}

string StatementMetaExternFunction::getName() const {
    return name;
}

string StatementMetaExternFunction::getGlobalName() const {
    string moduleName = this->moduleName;
    if (moduleName.empty())
        moduleName = "{UNDEFINED}";

    return format("{}.{}", moduleName, name);
}

string StatementMetaExternFunction::getModuleName() const {
    return moduleName;
}

void StatementMetaExternFunction::setModuleName(const string &moduleName) {
    if (this->moduleName.empty())
        this->moduleName = moduleName;
}

string StatementMetaExternFunction::getSymbolName() const {
    return symbolName;
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