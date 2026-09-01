#include "ValueTypeData.h"

ValueTypeData::ValueTypeData(shared_ptr<ValueType> elementValueType, shared_ptr<Expression> countExpression):
ValueType(ValueTypeKind::DATA),
elementValueType(elementValueType),
countExpression(countExpression) { }

shared_ptr<ValueType> ValueTypeData::getElementValueType() const {
    return elementValueType;
}

shared_ptr<Expression> ValueTypeData::getCountExpression() const {
    return countExpression;
}

void ValueTypeData::setModuleName(const string &moduleName) {
    if (elementValueType != nullptr)
        elementValueType->setModuleName(moduleName);
}

bool ValueTypeData::isEqual(shared_ptr<ValueType> other) const {
    // Are both ValueTypeData?
    shared_ptr<ValueTypeData> otherValueTypeData = dynamic_pointer_cast<ValueTypeData>(other);
    if (otherValueTypeData == nullptr)
        return false;

    // Both sub types can be null
    if (elementValueType == nullptr)
        return otherValueTypeData->getElementValueType() == nullptr;

    // Otherwise they must be equal
    return elementValueType->isEqual(otherValueTypeData->getElementValueType());
}

shared_ptr<ValueType> ValueTypeData::clone() const {
    return make_shared<ValueTypeData>(*this);
}