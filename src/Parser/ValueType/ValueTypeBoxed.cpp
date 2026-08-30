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
    // Sanity check
    if (other == nullptr)
        return false;

    // Check if it's a correct value type
    shared_ptr<ValueTypeBoxed> otherBoxed = dynamic_pointer_cast<ValueTypeBoxed>(other);
    if (otherBoxed == nullptr)
        return false;

    // Both boxed value types can be null
    if (boxedValueType == nullptr) {
        return otherBoxed->getBoxedValueType() == nullptr;
    // Otherwise they have to match
    } else {
        return boxedValueType->isEqual(otherBoxed->getBoxedValueType());
    }
}

shared_ptr<ValueType> ValueTypeBoxed::clone() const {
    return make_shared<ValueTypeBoxed>(*this);
}