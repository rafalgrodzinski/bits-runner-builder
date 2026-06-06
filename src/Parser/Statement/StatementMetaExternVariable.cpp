#include "StatementMetaExternVariable.h"

StatementMetaExternVariable::StatementMetaExternVariable(string identifier, shared_ptr<ValueType> valueType, shared_ptr<Location> location):
Statement(StatementKind::META_EXTERN_VARIABLE, location), identifier(std::move(identifier)), valueType(valueType) { }

string StatementMetaExternVariable::getIdentifier() const {
    return identifier;
}

shared_ptr<ValueType> StatementMetaExternVariable::getValueType() const {
    return valueType;
}