#include "ValueTypeBlob.h"

ValueTypeBlob::ValueTypeBlob(const string &name, const vector<shared_ptr<ValueType>> &namedValueTypes):
ValueType(ValueTypeKind::BLOB),
symbolName(make_shared<SymbolName>(name)),
namedValueTypes(namedValueTypes) { }

shared_ptr<SymbolName> ValueTypeBlob::getSymbolName() const {
    return symbolName;
}

optional<vector<string>> ValueTypeBlob::getNamedValueTypeKeys() {
    return namedValueTypeKeys;
}

vector<shared_ptr<ValueType>> ValueTypeBlob::getNamedValueTypes() {
    return namedValueTypes;
}

void ValueTypeBlob::setModuleName(const string &moduleName) {
    symbolName->setModuleName(moduleName);
}

bool ValueTypeBlob::isEqual(shared_ptr<ValueType> other) const {
    // Are both ValueTypeBlob?
    shared_ptr<ValueTypeBlob> valueTypeBlob = dynamic_pointer_cast<ValueTypeBlob>(other);
    if (valueTypeBlob == nullptr)
        return false;

    // Are the symbol names identical?
    if (!symbolName->isEqual(valueTypeBlob->getSymbolName()))
        return false;

    return true;
}

shared_ptr<ValueType> ValueTypeBlob::clone() const {
    return make_shared<ValueTypeBlob>(*this);
}