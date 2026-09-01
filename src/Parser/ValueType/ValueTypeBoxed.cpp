#include "ValueTypeBoxed.h"

ValueTypeBoxed::ValueTypeBoxed(const optional<string> &namedValueTypeKey, shared_ptr<ValueType> boxedValueType):
ValueType(ValueTypeKind::BOXED),
namedValueTypeKey(namedValueTypeKey),
boxedValueType(boxedValueType) { }

optional<string> ValueTypeBoxed::getNamedValueTypeKey() {
    return namedValueTypeKey;
}

shared_ptr<ValueType> ValueTypeBoxed::getBoxedValueType() {
    return boxedValueType;
}

void ValueTypeBoxed::setModuleName(const string &moduleName) {
    if (boxedValueType != nullptr)
        boxedValueType->setModuleName(moduleName);
}

bool ValueTypeBoxed::isEqual(shared_ptr<ValueType> other) const {
    // Check if types match
    shared_ptr<ValueTypeBoxed> otherValueTypeBoxed = other->boxed();
    if (otherValueTypeBoxed == nullptr)
        return false;

    // Both boxed value types can be null
    if (boxedValueType == nullptr)
        return otherValueTypeBoxed->getBoxedValueType() == nullptr;

    // Otherwise they have to match
    if (!boxedValueType->isEqual(otherValueTypeBoxed->getBoxedValueType()))
        return false;

    return true;
}

shared_ptr<ValueType> ValueTypeBoxed::clone() const {
    return make_shared<ValueTypeBoxed>(*this);
}