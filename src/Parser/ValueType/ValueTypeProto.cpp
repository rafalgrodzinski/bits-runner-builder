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
    // Are both ValueTypeProto?
    shared_ptr<ValueTypeProto> otherValueTypeProto = dynamic_pointer_cast<ValueTypeProto>(other);
    if (otherValueTypeProto == nullptr)
        return false;

    // Are the symbol names identical?
    return symbolName->isEqual(otherValueTypeProto->getSymbolName());
}

shared_ptr<ValueType> ValueTypeProto::clone() const {
    return make_shared<ValueTypeProto>(*this);
}