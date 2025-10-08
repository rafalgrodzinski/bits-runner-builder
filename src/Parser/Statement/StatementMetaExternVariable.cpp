#include "StatementMetaExternVariable.h"

StatementMetaExternVariable::StatementMetaExternVariable(string name, shared_ptr<ValueType> valueType):
Statement(StatementKind::META_EXTERN_VARIABLE), name(name), valueType(valueType) { }

string StatementMetaExternVariable::getName() {
    return name;
}

shared_ptr<ValueType> StatementMetaExternVariable::getValueType() {
    return valueType;
}