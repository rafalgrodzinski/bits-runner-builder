#ifndef VALUE_TYPE_H
#define VALUE_TYPE_H

#include <optional>

class Token;

using namespace std;

enum class ValueTypeKind {
    NONE,
    BOOL,
    SINT32,
    REAL32
};

class ValueType {
private:
    ValueTypeKind kind;

public:
    static shared_ptr<ValueType> NONE;
    static shared_ptr<ValueType> BOOL;
    static shared_ptr<ValueType> SINT32;
    static shared_ptr<ValueType> REAL32;
    static shared_ptr<ValueType> valueTypeForToken(shared_ptr<Token> token);

    ValueType(ValueTypeKind kind);
    ValueTypeKind getKind();
};

#endif