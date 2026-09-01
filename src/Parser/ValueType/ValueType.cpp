#include "ValueType.h"

ValueType::ValueType(ValueTypeKind kind):
kind(kind) { }

ValueTypeKind ValueType::getKind() const {
    return kind;
}

bool ValueType::isNumeric() const {
    switch (kind) {
        case ValueTypeKind::UINT:
        case ValueTypeKind::U8:
        case ValueTypeKind::U16:
        case ValueTypeKind::U32:
        case ValueTypeKind::U64:
    
        case ValueTypeKind::SINT:
        case ValueTypeKind::S8:
        case ValueTypeKind::S16:
        case ValueTypeKind::S32:
        case ValueTypeKind::S64:

        case ValueTypeKind::FLOAT:
        case ValueTypeKind::F32:
        case ValueTypeKind::F64:

        case ValueTypeKind::A:
            return true;

        default:
            break;
    }

    return false;
}

bool ValueType::isInteger() const {
    switch (kind) {
        case ValueTypeKind::UINT:
        case ValueTypeKind::U8:
        case ValueTypeKind::U16:
        case ValueTypeKind::U32:
        case ValueTypeKind::U64:

        case ValueTypeKind::SINT:
        case ValueTypeKind::S8:
        case ValueTypeKind::S16:
        case ValueTypeKind::S32:
        case ValueTypeKind::S64:

        case ValueTypeKind::A:
            return true;

        default:
            break;
    }

    return false;
}

bool ValueType::isUnsignedInteger() const {
    switch (kind) {
        case ValueTypeKind::UINT:
        case ValueTypeKind::U8:
        case ValueTypeKind::U16:
        case ValueTypeKind::U32:
        case ValueTypeKind::U64:
        case ValueTypeKind::A:
            return true;

        default:
            break;
    }

    return false;
}

bool ValueType::isSignedInteger() const {
    switch (kind) {
        case ValueTypeKind::SINT:
        case ValueTypeKind::S8:
        case ValueTypeKind::S16:
        case ValueTypeKind::S32:
        case ValueTypeKind::S64:
            return true;

        default:
            break;
    }

    return false;
}

bool ValueType::isFloat() const {
    switch (kind) {
        case ValueTypeKind::FLOAT:
        case ValueTypeKind::F32:
        case ValueTypeKind::F64:
            return true;

        default:
            break;
    }

    return false;
}

bool ValueType::isBool() const {
    return kind == ValueTypeKind::BOOL;
}

bool ValueType::isData() const {
    return kind == ValueTypeKind::DATA;
}

bool ValueType::isDataBool() const {
    /*if (isData() && getSubType()->isBool())
        return true;

    if (kind == ValueTypeKind::COMPOSITE) {
        vector<shared_ptr<ValueType>> elementTypes = *(getCompositeElementTypes());
        for (shared_ptr<ValueType> elementType : elementTypes) {
            if (!elementType->isBool())
                return false;
        }
        return true;
    }*/

    return false;
}

bool ValueType::isDataNumeric() const {
    /*if (isData() && getSubType()->isNumeric())
        return true;

    if (kind == ValueTypeKind::COMPOSITE) {
        vector<shared_ptr<ValueType>> elementTypes = *(getCompositeElementTypes());
        for (shared_ptr<ValueType> elementType : elementTypes) {
            if (!elementType->isNumeric())
                return false;
        }
        return true;
    }*/

    return false;
}

bool ValueType::isAddress() const {
    return kind == ValueTypeKind::A;
}

bool ValueType::isPointer() const {
    return kind == ValueTypeKind::PTR;
}

bool ValueType::isFunction() const {
    return kind == ValueTypeKind::FUN;
}

bool ValueType::isBlob() const {
    return kind == ValueTypeKind::BLOB;
}

bool ValueType::isEnum() const {
    return kind == ValueTypeKind::ENUM || kind == ValueTypeKind::ENUM_FIELD;
}

bool ValueType::isProto() const {
    return kind == ValueTypeKind::PROTO;
}

bool ValueType::isBoxed() const {
    return kind == ValueTypeKind::BOXED;
}

bool ValueType::isComposite() const {
    return kind == ValueTypeKind::COMPOSITE;
}