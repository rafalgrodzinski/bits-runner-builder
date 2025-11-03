#include "StatementMetaExternVariable.h"

StatementMetaExternVariable::StatementMetaExternVariable(string identifier, shared_ptr<ValueType> valueType):
Statement(StatementKind::META_EXTERN_VARIABLE), identifier(identifier), valueType(valueType) { }

string StatementMetaExternVariable::getIdentifier() {
    return identifier;
}

shared_ptr<ValueType> StatementMetaExternVariable::getValueType() {
    return valueType;
}