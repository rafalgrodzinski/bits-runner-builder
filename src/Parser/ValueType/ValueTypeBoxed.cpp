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

bool ValueTypeBoxed::isEqual(shared_ptr<ValueType> other) const {
    // Sanity check
    if (other == nullptr)
        return false;

    // Check if it's a correct value type
    shared_ptr<ValueTypeBoxed> otherBoxed = dynamic_pointer_cast<ValueTypeBoxed>(other);
    if (otherBoxed == nullptr)
        return false;

    // If both sub types can also be null
    if (subType == nullptr) {
        return otherBoxed->getSubType() == nullptr;
    // Otherwise they have to match
    } else {
        return subType->isEqual(otherBoxed->getSubType());
    }
}

shared_ptr<ValueType> ValueTypeBoxed::clone() const {
    return make_shared<ValueTypeBoxed>(*this);
}