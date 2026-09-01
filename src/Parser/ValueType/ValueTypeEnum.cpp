#include "ValueTypeEnum.h"

ValueTypeEnum::ValueTypeEnum(const string &name, const vector<shared_ptr<ValueType>> &namedValueTypes):
ValueType(ValueTypeKind::ENUM),
symbolName(make_shared<SymbolName>(name)),
namedValueTypes(namedValueTypes) { }

shared_ptr<SymbolName> ValueTypeEnum::getSymbolName() const {
    return symbolName;
}

vector<shared_ptr<ValueType>> ValueTypeEnum::getNamedValueTypes() {
    return namedValueTypes;
}

void ValueTypeEnum::setModuleName(const string &moduleName) {
    symbolName->setModuleName(moduleName);
}

bool ValueTypeEnum::isEqual(shared_ptr<ValueType> other) const {
    // Check if types match
    shared_ptr<ValueTypeEnum> otherValueTypeEnum = other->enumeration();
    if (otherValueTypeEnum == nullptr)
        return false;

    // Are the symbol names identical?
    if (!symbolName->isEqual(otherValueTypeEnum->getSymbolName()))
        return false;

    return true;
}

shared_ptr<ValueType> ValueTypeEnum::clone() const {
    return make_shared<ValueTypeEnum>(*this);
}