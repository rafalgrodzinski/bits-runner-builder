#ifndef VALUE_TYPE_H
#define VALUE_TYPE_H

#include <format>
#include <memory>
#include <optional>
#include <string>
#include <vector>

class Expression;
class Token;

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
friend class Analyzer;
friend class AnalyzerScope;

private:
    ValueTypeKind kind;

public:
    static shared_ptr<ValueType> NONE;
    static shared_ptr<ValueType> BOOL;
    static shared_ptr<ValueType> UINT;
    static shared_ptr<ValueType> U8;
    static shared_ptr<ValueType> U16;
    static shared_ptr<ValueType> U32;
    static shared_ptr<ValueType> U64;
    static shared_ptr<ValueType> SINT;
    static shared_ptr<ValueType> S8;
    static shared_ptr<ValueType> S16;
    static shared_ptr<ValueType> S32;
    static shared_ptr<ValueType> S64;
    static shared_ptr<ValueType> FLOAT;
    static shared_ptr<ValueType> F32;
    static shared_ptr<ValueType> F64;
    static shared_ptr<ValueType> A;

    static shared_ptr<ValueType> simpleForToken(shared_ptr<Token> token);

    ValueType();
    ValueType(ValueTypeKind kind);

    ValueTypeKind getKind() const;

    virtual void setModuleName(const string &moduleName) { }

    virtual bool isEqual(shared_ptr<ValueType> other) const;
    virtual shared_ptr<ValueType> clone() const;

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
};

#endif