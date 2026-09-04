#include "ValueType.h"
#include "ValueTypeBlob.h"
#include "ValueTypeBoxed.h"
#include "ValueTypeComposite.h"
#include "ValueTypeData.h"
#include "ValueTypeEnum.h"
#include "ValueTypeEnumField.h"
#include "ValueTypeFun.h"
#include "ValueTypeProto.h"
#include "ValueTypePtr.h"
#include "ValueTypeSimple.h"

ValueType::ValueType(ValueTypeKind kind):
kind(kind) { }

ValueTypeKind ValueType::getKind() const {
    return kind;
}

bool ValueType::isBlob() const {
    return kind == ValueTypeKind::BLOB;
}

bool ValueType::isBoxed() const {
    return kind == ValueTypeKind::BOXED;
}

bool ValueType::isComposite() const {
    return kind == ValueTypeKind::COMPOSITE;
}

bool ValueType::isData() const {
    return kind == ValueTypeKind::DATA;
}

bool ValueType::isEnum() const {
    return kind == ValueTypeKind::ENUM || kind == ValueTypeKind::ENUM_FIELD;
}

bool ValueType::isFun() const {
    return kind == ValueTypeKind::FUN;
}

bool ValueType::isProto() const {
    return kind == ValueTypeKind::PROTO;
}

bool ValueType::isPtr() const {
    return kind == ValueTypeKind::PTR;
}

bool ValueType::isSimple() const {
    switch (kind) {
        case ValueTypeKind::BOOL:
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
            return false;
    }
}

bool ValueType::isBool() const {
    return kind == ValueTypeKind::BOOL;
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

bool ValueType::isAddress() const {
    return kind == ValueTypeKind::A;
}

bool ValueType::isDataBool() {
    if (this->toData() == nullptr)
        return false;

    return toData()->getElementValueType()->isBool();
}

bool ValueType::isDataNumeric() {
    if (toData() == nullptr)
        return false;

    return toData()->getElementValueType()->isNumeric();
}

shared_ptr<ValueTypeBlob> ValueType::toBlob() {
    return dynamic_pointer_cast<ValueTypeBlob>(shared_from_this());
}

shared_ptr<ValueTypeBoxed> ValueType::toBoxed() {
    return dynamic_pointer_cast<ValueTypeBoxed>(shared_from_this());
}

shared_ptr<ValueTypeComposite> ValueType::toComposite() {
    return dynamic_pointer_cast<ValueTypeComposite>(shared_from_this());
}

shared_ptr<ValueTypeData> ValueType::toData() {
    return dynamic_pointer_cast<ValueTypeData>(shared_from_this());
}

shared_ptr<ValueTypeEnum> ValueType::toEnum() {
    return dynamic_pointer_cast<ValueTypeEnum>(shared_from_this());
}

shared_ptr<ValueTypeEnumField> ValueType::toEnumField() {
    return dynamic_pointer_cast<ValueTypeEnumField>(shared_from_this());
}

shared_ptr<ValueTypeFun> ValueType::toFun() {
    return dynamic_pointer_cast<ValueTypeFun>(shared_from_this());
}

shared_ptr<ValueTypeProto> ValueType::toProto() {
    return dynamic_pointer_cast<ValueTypeProto>(shared_from_this());
}

shared_ptr<ValueTypePtr> ValueType::toPtr() {
    return dynamic_pointer_cast<ValueTypePtr>(shared_from_this());
}

shared_ptr<ValueTypeSimple> ValueType::toSimple() {
    return dynamic_pointer_cast<ValueTypeSimple>(shared_from_this());
}
