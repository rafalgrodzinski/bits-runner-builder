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
    COMPOSITE,

    NAMED_TYPE
};

class ValueType {
friend class Analyzer;
friend class AnalyzerScope;

private:
    ValueTypeKind kind;
    string name;
    string moduleName;
    shared_ptr<ValueType> subType;
    shared_ptr<Expression> countExpression = nullptr;
    optional<vector<shared_ptr<ValueType>>> argumentTypes = {};
    shared_ptr<ValueType> returnType = nullptr;
    optional<vector<shared_ptr<ValueType>>> compositeElementTypes = {};
    optional<string> namedTypeKey = {};
    optional<vector<string>> namedTypeKeys = {};
    optional<vector<shared_ptr<ValueType>>> namedTypeValues = {};
    bool isVolatile = false;

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
    static shared_ptr<ValueType> data(shared_ptr<ValueType> subType, shared_ptr<Expression> countExpression);
    static shared_ptr<ValueType> proto(const string &protoName);
    static shared_ptr<ValueType> fun(const vector<shared_ptr<ValueType>> &argumentTypes, shared_ptr<ValueType> returnType);
    static shared_ptr<ValueType> ptr(shared_ptr<ValueType> subType, bool isVolatile);
    static shared_ptr<ValueType> composite(const vector<shared_ptr<ValueType>> &elementTypes, shared_ptr<Expression> countExpression);

    ValueType();
    ValueType(ValueTypeKind kind, const string &name = "");

    ValueTypeKind getKind() const;

    string getName() const;
    string getModuleName() const;
    virtual void setModuleName(const string &moduleName);
    string getGlobalName() const;

    bool getIsVolatile() const;
    // data, pointer, boxed
    shared_ptr<ValueType> getSubType() const;
    // data
    int getValueArg(); // TODO: remove
    shared_ptr<Expression> getCountExpression() const;
    // function
    optional<vector<shared_ptr<ValueType>>> getArgumentTypes() const;
    shared_ptr<ValueType> getReturnType() const;
    // composite
    optional<vector<shared_ptr<ValueType>>> getCompositeElementTypes() const;
    // boxed
    optional<string> getNamedTypeKey() const;
    optional<vector<string>> getNamedTypeKeys() const;
    optional<vector<shared_ptr<ValueType>>> getNamedTypeValues() const;

    virtual bool isEqual(shared_ptr<ValueType> other) const;

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
    bool isNamedType() const;
    bool isBoxedNamedType() const;

public:
    void setParent(weak_ptr<ValueType> parent);
    weak_ptr<ValueType> getParent();

private:
    weak_ptr<ValueType> parent;
};

#endif