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
    // Check if types match
    shared_ptr<ValueTypeComposite> otherValueTypeComposite = other->composite();
    if (otherValueTypeComposite == nullptr)
        return false;

    // Check if number of elements match
    if (elementValueTypes.size() != otherValueTypeComposite->getElementValueTypes().size())
        return false;

    // TODO: We probably want this
    /*
    // Check if types of elements match
    for (int i=0; i<elementValueTypes.size(); i++) {
        if (!elementValueTypes[i]->isEqual(otherValueTypeComposite->getElementValueTypes()[i]))
            return false;
    }
    */

    return true;
}

shared_ptr<ValueType> ValueTypeComposite::clone() const {
    return make_shared<ValueTypeComposite>(*this);
}