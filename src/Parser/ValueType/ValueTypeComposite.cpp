#include "ValueTypeComposite.h"

ValueTypeComposite::ValueTypeComposite(const vector<shared_ptr<ValueType>> &elementValueTypes, shared_ptr<Expression> countExpression):
ValueType(ValueTypeKind::COMPOSITE),
elementValueTypes(elementValueTypes),
countExpression(countExpression) { }

vector<shared_ptr<ValueType>> ValueTypeComposite::getElementValueTypes() const {
    return elementValueTypes;
}

shared_ptr<Expression> ValueTypeComposite::getCountExpression() const {
    return countExpression;
}

void ValueTypeComposite::setModuleName(const string &moduleName) {
    for (shared_ptr<ValueType> elementValueType : elementValueTypes)
        elementValueType->setModuleName(moduleName);
}

bool ValueTypeComposite::isEqual(shared_ptr<ValueType> other) const {
    return false;
}

shared_ptr<ValueType> ValueTypeComposite::clone() const {
    return make_shared<ValueTypeComposite>(*this);
}