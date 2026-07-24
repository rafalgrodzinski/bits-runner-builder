#include "ValueTypeEnumField.h"

#include "ValueTypeEnum.h"

ValueTypeEnumField::ValueTypeEnumField(const string &name, const vector<shared_ptr<ValueType>> namedValueTypes):
ValueType(ValueTypeKind::ENUM_FIELD),
symbolName(make_shared<SymbolName>(name)),
namedValueTypes(namedValueTypes) { }

shared_ptr<SymbolName> ValueTypeEnumField::getSymbolName() const {
    return symbolName;
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

bool ValueTypeEnumField::canImplicitCastTo(shared_ptr<ValueType> other) const {
    // Are these identical?
    if (isEqual(other))
        return true;

    // Is this a field of a parent enum?
    shared_ptr<ValueTypeEnum> valueTypeEnum = dynamic_pointer_cast<ValueTypeEnum>(other);
    if (valueTypeEnum == nullptr)
        return false;

    if (!symbolName->getGlobalName().starts_with(valueTypeEnum->getSymbolName()->getGlobalName()))
        return false;

    // Are the named value types identical?
    //return getNamedTypesMap() == valueTypeEnum->getNamedTypesMap();
    return true;
}

shared_ptr<ValueType> ValueTypeEnumField::getPayloadValueType() const {
    return payloadValueType;
}

/*map<string, shared_ptr<ValueType>> ValueTypeEnumField::getNamedTypesMap() const {
    map<string, shared_ptr<ValueType>> namedTypesMap;

    if (namedValueTypeKeys.size() != namedValueTypes.size())
        return namedTypesMap;;

    for (int i=0; i<namedValueTypeKeys.size(); i++)
        namedTypesMap[namedValueTypeKeys[i]] = namedValueTypes[i];

    return namedTypesMap;
}*/