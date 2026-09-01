#include "ValueTypeFun.h"
#include "Parser/SymbolName.h"

ValueTypeFun::ValueTypeFun(
    const vector<shared_ptr<ValueType>> &argumentValueTypes,
    shared_ptr<ValueType> returnValueType
):
ValueType(ValueTypeKind::FUN),
argumentValueTypes(argumentValueTypes),
returnValueType(returnValueType) { }

vector<shared_ptr<ValueType>> ValueTypeFun::getArgumentValueTypes() {
    return argumentValueTypes;
}

shared_ptr<ValueType> ValueTypeFun::getReturnValueType() {
    return returnValueType;
}

void ValueTypeFun::setModuleName(const string &moduleName) {
    for (shared_ptr<ValueType> valueType : argumentValueTypes)
        valueType->setModuleName(moduleName);

    returnValueType->setModuleName(moduleName);
}

bool ValueTypeFun::isEqual(shared_ptr<ValueType> other) const {
    // Sanity check
    if (other == nullptr)
        return false;

    // Both must be ValueTypeFun
    shared_ptr<ValueTypeFun> otherFun = dynamic_pointer_cast<ValueTypeFun>(other);
    if (otherFun == nullptr)
        return false;

    // Does arguments count match?
    if (argumentValueTypes.size() != otherFun->getArgumentValueTypes().size())
        return false;

    // Do argument types match?
    for (int i=0; i<argumentValueTypes.size(); i++) {
        if (!argumentValueTypes.at(i)->isEqual(otherFun->getArgumentValueTypes().at(i)))
            return false;
    }

    // do the return types match?
    if (!returnValueType->isEqual(otherFun->getReturnValueType()))
        return false;

    return true;
}

shared_ptr<ValueType> ValueTypeFun::clone() const {
    return make_shared<ValueTypeFun>(*this);
}