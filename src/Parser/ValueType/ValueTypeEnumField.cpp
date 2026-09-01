#include "ValueTypeEnumField.h"

#include "ValueTypeEnum.h"

ValueTypeEnumField::ValueTypeEnumField(const string &name, const vector<shared_ptr<ValueType>> namedValueTypes):
ValueType(ValueTypeKind::ENUM_FIELD),
symbolName(make_shared<SymbolName>(name)),
namedValueTypes(namedValueTypes) { }

shared_ptr<SymbolName> ValueTypeEnumField::getSymbolName() const {
    return symbolName;
}

vector<shared_ptr<ValueType>> ValueTypeEnumField::getNamedValueTypes() const {
    return namedValueTypes;
}

shared_ptr<ValueType> ValueTypeEnumField::getPayloadValueType() const {
    return payloadValueType;
}

void ValueTypeEnumField::setModuleName(const string &moduleName) {
    symbolName->setModuleName(moduleName);
}

bool ValueTypeEnumField::isEqual(shared_ptr<ValueType> other) const {
    // Check types
    shared_ptr<ValueTypeEnumField> otherValueTypeEnumField = other->enumField();
    if (otherValueTypeEnumField == nullptr)
        return false;

    // Are the symbol names identical?
    if (!symbolName->isEqual(otherValueTypeEnumField->getSymbolName()))
        return false;

    return true;
}

shared_ptr<ValueType> ValueTypeEnumField::clone() const {
    return make_shared<ValueTypeEnumField>(*this);
}