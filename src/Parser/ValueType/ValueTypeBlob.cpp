#include "ValueTypeBlob.h"

ValueTypeBlob::ValueTypeBlob(string name, optional<vector<shared_ptr<ValueType>>> namedTypeValues):
ValueType(ValueTypeKind::BLOB), name(name), namedTypeValues(namedTypeValues) {
    size_t pos = name.find('.');
    if (pos != string::npos) {
        this->moduleName = name.substr(0, pos);
        this->name = name.substr(pos + 1, name.size() - 1);
    } else {
        this->name = name;
    }
}

string ValueTypeBlob::getName() const {
    if (!moduleName.empty())
        return format("{}.{}", moduleName, name);
    return name;
}

string ValueTypeBlob::getModuleName() const {
    return moduleName;
}

void ValueTypeBlob::setModuleName(string moduleName) {
    this->moduleName = moduleName;
}

bool ValueTypeBlob::isEqual(shared_ptr<ValueType> other) const {
    switch (other->getKind()) {
        case ValueTypeKind::BLOB: {
            shared_ptr<ValueTypeBlob> otherBlob = dynamic_pointer_cast<ValueTypeBlob>(other);
            return getName() == otherBlob->getName();
        }
        case ValueTypeKind::BOXED: {
            shared_ptr<ValueType> otherBoxed = dynamic_pointer_cast<ValueType>(other);
            return isEqual(other->getSubType());
        }
        default:
            break;
    }

    return false;
}