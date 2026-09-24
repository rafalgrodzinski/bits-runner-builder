#ifndef VALUE_TYPE_H
#define VALUE_TYPE_H

#include <memory>
#include <string>

class Location;
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

enum class ValueTypeKind {
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

    NONE,
    BOOL,
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

class ValueType: public std::enable_shared_from_this<ValueType> {
public:
    ValueType(ValueTypeKind kind, std::shared_ptr<Location> location);
    virtual ~ValueType() = default;

    ValueTypeKind getKind() const;
    std::shared_ptr<Location> getLocation() const;

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

    std::shared_ptr<ValueTypeBlob> toBlob();
    std::shared_ptr<ValueTypeBoxed> toBoxed();
    std::shared_ptr<ValueTypeComposite> toComposite();
    std::shared_ptr<ValueTypeData> toData();
    std::shared_ptr<ValueTypeEnum> toEnum();
    std::shared_ptr<ValueTypeEnumField> toEnumField();
    std::shared_ptr<ValueTypeFun> toFun();
    std::shared_ptr<ValueTypeProto> toProto();
    std::shared_ptr<ValueTypePtr> toPtr();
    std::shared_ptr<ValueTypeSimple> toSimple();

    virtual void setModuleName(const std::string &moduleName) { }
    virtual bool isEqual(std::shared_ptr<ValueType> other) const = 0;
    virtual std::shared_ptr<ValueType> clone() const = 0;

private:
    ValueTypeKind kind;
    std::shared_ptr<Location> location;
};

#endif