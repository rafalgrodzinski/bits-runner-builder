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
    // Are both ValueTypeEnum?
    shared_ptr<ValueTypeEnum> valueTypeEnum = dynamic_pointer_cast<ValueTypeEnum>(other);
    if (valueTypeEnum == nullptr)
        return false;

    // Are the symbol names identical?
    if (symbolName->getGlobalName() != valueTypeEnum->getSymbolName()->getGlobalName())
        return false;

    // Are the named value types identical?

    return true;
}

shared_ptr<ValueType> ValueTypeEnum::clone() const {
    return make_shared<ValueTypeEnum>(*this);
}