#ifndef VALUE_TYPE_H
#define VALUE_TYPE_H

#include <optional>
#include <string>
#include <vector>

class Token;
class Expression;

using namespace std;

enum class ValueTypeKind {
    NONE,
    BOOL,
    U8,
    U32,
    U64,
    S8,
    S32,
    S64,
    F32,
    F64,
    DATA,
    BLOB,
    FUN,
    PTR,
    LITERAL
};

class ValueType {
private:
    ValueTypeKind kind;
    shared_ptr<ValueType> subType;
    int valueArg;
    string typeName;
    vector<shared_ptr<ValueType>> argTypes;
    shared_ptr<ValueType> retType;

public:
    static shared_ptr<ValueType> NONE;
    static shared_ptr<ValueType> BOOL;
    static shared_ptr<ValueType> U8;
    static shared_ptr<ValueType> U32;
    static shared_ptr<ValueType> U64;
    static shared_ptr<ValueType> S8;
    static shared_ptr<ValueType> S32;
    static shared_ptr<ValueType> S64;
    static shared_ptr<ValueType> F32;
    static shared_ptr<ValueType> F64;
    static shared_ptr<ValueType> LITERAL;
    static shared_ptr<ValueType> valueTypeForFun(vector<shared_ptr<ValueType>> argTypes, shared_ptr<ValueType> retType);
    static shared_ptr<ValueType> valueTypeForToken(shared_ptr<Token> token, shared_ptr<ValueType> subType, shared_ptr<Expression> sizeExpression, string typeName);

    ValueType();
    ValueType(ValueTypeKind kind, shared_ptr<ValueType> subType, int valueArg, string typeName);

    ValueTypeKind getKind();
    shared_ptr<ValueType> getSubType();
    int getValueArg();
    string getTypeName();
    vector<shared_ptr<ValueType>> getArgTypes();
    shared_ptr<ValueType> getRetType();
};

#endif