#include "ValueTypeBlob.h"
#include "Parser/SymbolName.h"

ValueTypeBlob::ValueTypeBlob(
    const string &name,
    const vector<shared_ptr<ValueType>> &namedValueTypes,
    shared_ptr<Location> location):
ValueType(ValueTypeKind::BLOB, location),
symbolName(make_shared<SymbolName>(name)),
namedValueTypes(namedValueTypes) { }

shared_ptr<SymbolName> ValueTypeBlob::getSymbolName() const {
    return symbolName;
}

optional<vector<string>> ValueTypeBlob::getNamedValueTypeKeys() const {
    return namedValueTypeKeys;
}

vector<shared_ptr<ValueType>> ValueTypeBlob::getNamedValueTypes() const {
    return namedValueTypes;
}

void ValueTypeBlob::setModuleName(const string &moduleName) {
    symbolName->setModuleName(moduleName);
    for (shared_ptr<ValueType> namedValueType : namedValueTypes)
        namedValueType->setModuleName(moduleName);
}

bool ValueTypeBlob::isEqual(shared_ptr<ValueType> other) const {
    // Check if types match
    shared_ptr<ValueTypeBlob> otherValueTypeBlob = other->toBlob();
    if (otherValueTypeBlob == nullptr)
        return false;

    // Are the symbol names identical?
    if (!symbolName->isEqual(otherValueTypeBlob->getSymbolName()))
        return false;

    return true;
}

shared_ptr<ValueType> ValueTypeBlob::clone() const {
    return make_shared<ValueTypeBlob>(*this);
}