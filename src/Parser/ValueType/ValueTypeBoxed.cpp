#include "ValueTypeBoxed.h"

ValueTypeBoxed::ValueTypeBoxed(const optional<string> &namedValueTypeKey, shared_ptr<ValueType> subType):
ValueType(ValueTypeKind::BOXED),
namedValueTypeKey(namedValueTypeKey),
subType(subType) { }

optional<string> ValueTypeBoxed::getNamedValueTypeKey() {
    return namedValueTypeKey;
}

shared_ptr<ValueType> ValueTypeBoxed::getSubType() {
    return subType;
}