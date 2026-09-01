#include "ValueTypeData.h"
#include "Parser/Expression/Expression.h"

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

    /*
            case ValueTypeKind::DATA: {
            // first check the types
            if (!other->isData() || !subType->isEqual(other->getSubType()))
                return false;

            // then check the elements count
            shared_ptr<ExpressionLiteral> thisCountLiteralExpression = dynamic_pointer_cast<ExpressionLiteral>(countExpression);
            shared_ptr<ExpressionLiteral> thatCountLiteralExpression = dynamic_pointer_cast<ExpressionLiteral>(other->getCountExpression());

            // if both have no size specified, then it's good
            if (thisCountLiteralExpression == nullptr && thatCountLiteralExpression == nullptr)
                return true;

            // otherwise check that both have some sizes
            if (thisCountLiteralExpression == nullptr || thatCountLiteralExpression == nullptr)
                return false;
            // sizes must be unsigned integers
            bool isThisTypeValid = thisCountLiteralExpression->getValueType()->isUnsignedInteger();
            bool isThatTypeValid = thatCountLiteralExpression->getValueType()->isUnsignedInteger();
            if (!isThisTypeValid || !isThatTypeValid)
                return false;

            int thisSize = thisCountLiteralExpression->getUIntValue();
            int thatSize = thatCountLiteralExpression->getUIntValue();

            return thisSize == thatSize;
        }
    */
}

shared_ptr<ValueType> ValueTypeData::clone() const {
    return make_shared<ValueTypeData>(*this);
}