#include "ValueTypePtr.h"

ValueTypePtr::ValueTypePtr(shared_ptr<ValueType> pointeeValueType, bool isVolatile):
ValueType(ValueTypeKind::PTR),
pointeeValueType(pointeeValueType),
isVolatile(isVolatile) { }

shared_ptr<ValueType> ValueTypePtr::getPointeeValueType() const {
    return pointeeValueType;
}

bool ValueTypePtr::getIsVolatile() const {
    return isVolatile;
}

void ValueTypePtr::setModuleName(const string &moduleName) {
    if (pointeeValueType != nullptr)
        pointeeValueType->setModuleName(moduleName);
}

bool ValueTypePtr::isEqual(shared_ptr<ValueType> other) const {
    // Check if types match
    shared_ptr<ValueTypePtr> otherValueTypePtr = other->toPtr();
    if (otherValueTypePtr == nullptr)
        return false;

    // Both can be null
    if (pointeeValueType == nullptr)
        return otherValueTypePtr->getPointeeValueType() == nullptr;

    // Otherwise pointee types must be equal
    return pointeeValueType->isEqual(otherValueTypePtr->getPointeeValueType());
}

shared_ptr<ValueType> ValueTypePtr::clone() const {
    return make_shared<ValueTypePtr>(*this);
}