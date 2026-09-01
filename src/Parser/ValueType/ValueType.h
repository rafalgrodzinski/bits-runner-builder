#ifndef VALUE_TYPE_H
#define VALUE_TYPE_H

#include <memory>
#include <string>

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
    PTR,

    DATA,
    BLOB,
    ENUM,
    ENUM_FIELD,
    PROTO,
    BOXED,
    FUN,
    COMPOSITE
};

class ValueType {
public:
    ValueType(ValueTypeKind kind);
    virtual ~ValueType() = default;

    ValueTypeKind getKind() const;

    bool isNumeric() const;
    bool isInteger() const;
    bool isUnsignedInteger() const;
    bool isSignedInteger() const;
    bool isFloat() const;
    bool isBool() const;

    bool isData() const;
    bool isDataBool() const;
    bool isDataNumeric() const;

    bool isAddress() const;
    bool isPointer() const;
    bool isFunction() const;
    bool isBlob() const;
    bool isEnum() const;
    bool isProto() const;
    bool isBoxed() const;
    bool isComposite() const;

    virtual void setModuleName(const string &moduleName) { }
    virtual bool isEqual(shared_ptr<ValueType> other) const = 0;
    virtual shared_ptr<ValueType> clone() const = 0;

private:
    ValueTypeKind kind;
};

#endif