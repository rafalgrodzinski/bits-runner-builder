#ifndef VALUE_TYPE_H
#define VALUE_TYPE_H

#include <optional>
#include <string>
#include <vector>
#include <memory>

class Expression;
class Token;

using namespace std;

enum class ValueTypeKind {
    NONE,
    BOOL,
    INT,
    U8,
    U32,
    U64,
    S8,
    S32,
    S64,
    FLOAT,
    F32,
    F64,
    DATA,
    BLOB,
    FUN,
    PTR,
    COMPOSITE
};

class ValueType {
private:
    ValueTypeKind kind;
    shared_ptr<ValueType> subType;
    shared_ptr<Expression> countExpression;
    optional<vector<shared_ptr<ValueType>>> argumentTypes;
    shared_ptr<ValueType> returnType;
    optional<string> blobName;
    optional<vector<shared_ptr<ValueType>>> compositeElementTypes;

public:
    static shared_ptr<ValueType> NONE;
    static shared_ptr<ValueType> BOOL;
    static shared_ptr<ValueType> INT;
    static shared_ptr<ValueType> U8;
    static shared_ptr<ValueType> U32;
    static shared_ptr<ValueType> U64;
    static shared_ptr<ValueType> S8;
    static shared_ptr<ValueType> S32;
    static shared_ptr<ValueType> S64;
    static shared_ptr<ValueType> FLOAT;
    static shared_ptr<ValueType> F32;
    static shared_ptr<ValueType> F64;

    static shared_ptr<ValueType> simpleForToken(shared_ptr<Token> token);
    static shared_ptr<ValueType> data(shared_ptr<ValueType> subType, shared_ptr<Expression> countExpression);
    static shared_ptr<ValueType> blob(string blobName);
    static shared_ptr<ValueType> fun(vector<shared_ptr<ValueType>> argumentTypes, shared_ptr<ValueType> returnType);
    static shared_ptr<ValueType> ptr(shared_ptr<ValueType> subType);
    static shared_ptr<ValueType> composite(vector<shared_ptr<ValueType>> elementTypes, shared_ptr<Expression> countExpression);

    ValueType();
    ValueType(ValueTypeKind kind);

    ValueTypeKind getKind();
    // data, pointer
    shared_ptr<ValueType> getSubType();
    // data
    int getValueArg(); // TODO: remove
    shared_ptr<Expression> getCountExpression();
    // function
    optional<vector<shared_ptr<ValueType>>> getArgumentTypes();
    shared_ptr<ValueType> getReturnType();
    // blob
    optional<string> getBlobName();
    // composite
    optional<vector<shared_ptr<ValueType>>> getCompositeElementTypes();

    bool isEqual(shared_ptr<ValueType> other);
    bool isNumeric();
    bool isInteger();
    bool isUnsignedInteger();
    bool isSignedInteger();
    bool isFloat();
    bool isBool();
    bool isData();
    bool isDataBool();
    bool isDataNumeric();
    bool isPointer();
    bool isFunction();
    bool isBlob();
};

#endif