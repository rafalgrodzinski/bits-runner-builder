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
    shared_ptr<ValueTypeEnumField> valueTypeEnumField = dynamic_pointer_cast<ValueTypeEnumField>(other);
    if (valueTypeEnumField == nullptr)
        return false;
    
    return symbolName->getGlobalName() == valueTypeEnumField->getSymbolName()->getGlobalName();
}