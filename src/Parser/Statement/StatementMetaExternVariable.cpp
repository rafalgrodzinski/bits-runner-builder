#include "StatementMetaExternVariable.h"

StatementMetaExternVariable::StatementMetaExternVariable(const string &identifier, shared_ptr<ValueType> valueType, shared_ptr<Location> location):
Statement(StatementKind::META_EXTERN_VARIABLE, location), valueType(valueType) { 
    symbolName = identifier;

    size_t pos = identifier.find('.');
    if (pos != string::npos) {
        this->moduleName = identifier.substr(0, pos);
        this->identifier = identifier.substr(pos + 1, identifier.size());
    } else {
        this->identifier = identifier;
    }
}

string StatementMetaExternVariable::getIdentifier() const {
    return identifier;
}

string StatementMetaExternVariable::getGlobalIdentifier() const {
    string moduleName = this->moduleName;
    if (moduleName.empty())
        moduleName = "{UNDEFINED}";

    return format("{}.{}", moduleName, identifier);
}

string StatementMetaExternVariable::getModuleName() const {
    return moduleName;
}

void StatementMetaExternVariable::setModuleName(const string &moduleName) {
    if (this->moduleName.empty())
        this->moduleName = moduleName;
}

string StatementMetaExternVariable::getSymbolName() const {
    return symbolName;
}

shared_ptr<ValueType> StatementMetaExternVariable::getValueType() const {
    return valueType;
}