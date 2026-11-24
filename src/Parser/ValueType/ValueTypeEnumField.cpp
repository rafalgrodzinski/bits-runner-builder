#include "ValueTypeEnumField.h"

#include "ValueTypeEnum.h"

using namespace std;

// MARK: - Public

ValueTypeEnumField::ValueTypeEnumField(
    const string &enumFieldName,
    const string &enumName,
    const vector<shared_ptr<ValueType>> namedValueTypes,
    shared_ptr<Location> location
):
ValueType(ValueTypeKind::ENUM_FIELD, location),
enumFieldSymbolName(make_shared<SymbolName>(enumFieldName)),
enumSymbolName(make_shared<SymbolName>(enumName)),
namedValueTypes(namedValueTypes) { }

shared_ptr<SymbolName> ValueTypeEnumField::getEnumFieldSymbolName() const {
    return enumFieldSymbolName;
}

std::shared_ptr<SymbolName> ValueTypeEnumField::getEnumSymbolName() const {
    return enumSymbolName;
}

vector<shared_ptr<ValueType>> ValueTypeEnumField::getNamedValueTypes() const {
    return namedValueTypes;
}

shared_ptr<ValueType> ValueTypeEnumField::getPayloadValueType() const {
    return payloadValueType;
}

std::shared_ptr<ValueTypeEnum> ValueTypeEnumField::getEnumValueType() const {
    return make_shared<ValueTypeEnum>(
        enumSymbolName->getGlobalName(),
        namedValueTypes,
        getLocation()
    );
}

void ValueTypeEnumField::setModuleName(const string &moduleName) {
    enumFieldSymbolName->setModuleName(moduleName);
    enumSymbolName->setModuleName(moduleName);
    for (shared_ptr<ValueType> namedValueType : namedValueTypes)
        namedValueType->setModuleName(moduleName);
}

bool ValueTypeEnumField::isEqual(shared_ptr<ValueType> other) const {
    // Check types
    shared_ptr<ValueTypeEnumField> otherValueTypeEnumField = other->toEnumField();
    if (otherValueTypeEnumField == nullptr)
        return false;

    // Are the symbol names identical?
    if (!enumFieldSymbolName->isEqual(otherValueTypeEnumField->getEnumFieldSymbolName()))
        return false;

    return true;
}

shared_ptr<ValueType> ValueTypeEnumField::clone() const {
    return make_shared<ValueTypeEnumField>(*this);
}