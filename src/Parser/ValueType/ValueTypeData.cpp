#include "ValueTypeData.h"
#include "Parser/Expression/Expression.h"
#include "Parser/Expression/ExpressionLiteral.h"

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
    // Check if types match
    shared_ptr<ValueTypeData> otherValueTypeData = other->toData();
    if (otherValueTypeData == nullptr)
        return false;

    // Both element types can be null
    if (elementValueType == nullptr)
        return otherValueTypeData->getElementValueType() == nullptr;

    // Otherwise they must be equal
    if (!elementValueType->isEqual(otherValueTypeData->getElementValueType()))
        return false;

    // Both may have count unspecified
    if (countExpression == nullptr)
        return otherValueTypeData->getCountExpression() == nullptr;

    // Otherwise they have to match
    shared_ptr<ExpressionLiteral> thisCountExpression = dynamic_pointer_cast<ExpressionLiteral>(countExpression);
    shared_ptr<ExpressionLiteral> otherCountExpression = dynamic_pointer_cast<ExpressionLiteral>(otherValueTypeData->getCountExpression());
    if (thisCountExpression == nullptr || otherCountExpression == nullptr)
        return false;

    if (thisCountExpression->getUIntValue() != otherCountExpression->getUIntValue())
        return false;

    return true;
}

shared_ptr<ValueType> ValueTypeData::clone() const {
    return make_shared<ValueTypeData>(*this);
}