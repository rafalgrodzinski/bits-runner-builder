#ifndef VALUE_TYPE_H
#define VALUE_TYPE_H

#include <optional>
#include <string>

class Token;

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
    R32,
    DATA,
    BLOB,
    PTR
};

class ValueType {
private:
    ValueTypeKind kind;
    shared_ptr<ValueType> subType;
    int valueArg;
    string typeName;

public:
    static shared_ptr<ValueType> NONE;
    static shared_ptr<ValueType> BOOL;
    static shared_ptr<ValueType> U8;
    static shared_ptr<ValueType> U32;
    static shared_ptr<ValueType> U64;
    static shared_ptr<ValueType> S8;
    static shared_ptr<ValueType> S32;
    static shared_ptr<ValueType> S64;
    static shared_ptr<ValueType> R32;
    static shared_ptr<ValueType> valueTypeForToken(shared_ptr<Token> token, shared_ptr<ValueType> subType, int valueArg, string typeName);

    ValueType();
    ValueType(ValueTypeKind kind, shared_ptr<ValueType> subType, int valueArg, string typeName);
    ValueTypeKind getKind();
    shared_ptr<ValueType> getSubType();
    int getValueArg();
    string getTypeName();
};

#endif