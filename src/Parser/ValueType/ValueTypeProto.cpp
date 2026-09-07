#include "ValueTypeProto.h"

ValueTypeProto::ValueTypeProto(const string &name):
ValueType(ValueTypeKind::PROTO),
symbolName(make_shared<SymbolName>(name)) { }

shared_ptr<SymbolName> ValueTypeProto::getSymbolName() const {
    return symbolName;
}

void ValueTypeProto::setModuleName(const string &moduleName) {
    symbolName->setModuleName(moduleName);
}

bool ValueTypeProto::isEqual(shared_ptr<ValueType> other) const {
    // Check types
    shared_ptr<ValueTypeProto> otherValueTypeProto = other->toProto();
    if (otherValueTypeProto == nullptr)
        return false;

    // Are the symbol names identical?
    if (!symbolName->isEqual(otherValueTypeProto->getSymbolName()))
        return false;

    return true;
}

shared_ptr<ValueType> ValueTypeProto::clone() const {
    return make_shared<ValueTypeProto>(*this);
}