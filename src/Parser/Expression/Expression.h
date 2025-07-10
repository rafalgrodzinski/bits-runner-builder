#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <iostream>

class Token;
class ValueType;

using namespace std;

enum class ExpressionKind {
    LITERAL,
    ARRAY_LITERAL,
    GROUPING,
    BINARY,
    IF_ELSE,
    VAR,
    CALL,
    BLOCK
};

class Expression {
private:
    ExpressionKind kind;

protected:
    shared_ptr<ValueType> valueType;

public:
    Expression(ExpressionKind kind, shared_ptr<ValueType> valueType);
    virtual ~Expression() { }
    ExpressionKind getKind();
    shared_ptr<ValueType> getValueType();
};

#endif