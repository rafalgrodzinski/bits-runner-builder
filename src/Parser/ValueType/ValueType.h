#ifndef VALUE_TYPE_H
#define VALUE_TYPE_H

#include <memory>
#include <string>

class ValueTypeBlob;
class ValueTypeBoxed;
class ValueTypeComposite;
class ValueTypeData;
class ValueTypeEnum;
class ValueTypeEnumField;
class ValueTypeFun;
class ValueTypeProto;
class ValueTypePtr;
class ValueTypeSimple;

using namespace std;

enum class ValueTypeKind {
    NONE,
    BOOL,

    UINT,
    U8,
    U16,
    U32,
    U64,

    SINT,
    S8,
    S16,
    S32,
    S64,

    FLOAT,
    F32,
    F64,

    A,

    BLOB,
    BOXED,
    COMPOSITE,
    DATA,
    ENUM,
    ENUM_FIELD,
    FUN,
    PROTO,
    PTR
};

class ValueType: public enable_shared_from_this<ValueType> {
public:
    ValueType(ValueTypeKind kind);
    virtual ~ValueType() = default;

    ValueTypeKind getKind() const;

    bool isBlob() const;
    bool isBoxed() const;
    bool isComposite() const;
    bool isData() const;
    bool isEnum() const;
    bool isFun() const;
    bool isProto() const;
    bool isPtr() const;
    bool isSimple() const;

    bool isBool() const;
    bool isNumeric() const;
    bool isInteger() const;
    bool isUnsignedInteger() const;
    bool isSignedInteger() const;
    bool isFloat() const;
    bool isAddress() const;
    bool isDataBool();
    bool isDataNumeric();

    shared_ptr<ValueTypeBlob> toBlob();
    shared_ptr<ValueTypeBoxed> toBoxed();
    shared_ptr<ValueTypeComposite> toComposite();
    shared_ptr<ValueTypeData> toData();
    shared_ptr<ValueTypeEnum> toEnum();
    shared_ptr<ValueTypeEnumField> toEnumField();
    shared_ptr<ValueTypeFun> toFun();
    shared_ptr<ValueTypeProto> toProto();
    shared_ptr<ValueTypePtr> toPtr();
    shared_ptr<ValueTypeSimple> toSimple();

    virtual void setModuleName(const string &moduleName) { }
    virtual bool isEqual(shared_ptr<ValueType> other) const = 0;
    virtual shared_ptr<ValueType> clone() const = 0;

private:
    ValueTypeKind kind;
};

#endif