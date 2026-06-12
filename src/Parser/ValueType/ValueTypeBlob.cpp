#include "ValueTypeBlob.h"

ValueTypeBlob::ValueTypeBlob(string name, optional<vector<shared_ptr<ValueType>>> namedTypeValues):
ValueType(ValueTypeKind::BLOB), namedTypeValues(namedTypeValues) {
    size_t pos = name.find('.');
    if (pos != string::npos) {
        this->moduleName = name.substr(0, pos);
        this->name = name.substr(pos + 1, name.size() - 1);
    } else {
        this->name = name;
    }
}

bool ValueTypeBlob::isEqual(shared_ptr<ValueType> other) const {
    switch (other->getKind()) {
        case ValueTypeKind::BLOB: {
            return getGlobalName() == other->getGlobalName();
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