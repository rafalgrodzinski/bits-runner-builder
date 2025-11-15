#ifndef VALUE_TYPE_H
#define VALUE_TYPE_H

#include <optional>
#include <string>
#include <vector>

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
    LITERAL,
    COMPOSITE
};

class ValueType {
private:
    ValueTypeKind kind;
    shared_ptr<ValueType> subType;
    string blobName;
    vector<shared_ptr<ValueType>> argumentTypes;
    shared_ptr<ValueType> returnType;
    shared_ptr<Expression> sizeExpression;

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
    static shared_ptr<ValueType> LITERAL;
    static shared_ptr<ValueType> COMPOSITE;
    static shared_ptr<ValueType> data(shared_ptr<ValueType> subType, shared_ptr<Expression> sizeExpression);
    static shared_ptr<ValueType> blob(string blobName);
    static shared_ptr<ValueType> fun(vector<shared_ptr<ValueType>> argumentTypes, shared_ptr<ValueType> returnType);
    static shared_ptr<ValueType> ptr(shared_ptr<ValueType> subType);
    static shared_ptr<ValueType> simpleForToken(shared_ptr<Token> token);

    ValueType();
    ValueType(ValueTypeKind kind);

    ValueTypeKind getKind();
    shared_ptr<ValueType> getSubType();
    int getValueArg();
    shared_ptr<Expression> getSizeExpression();
    vector<shared_ptr<ValueType>> getArgumentTypes();
    shared_ptr<ValueType> getReturnType();
    string getBlobName();

    bool isEqual(shared_ptr<ValueType> other);
    bool isNumeric();
    bool isInteger();
    bool isBool();
    bool isData();
    bool isPointer();
    bool isFunction();
    bool isBlob();
};

#endif