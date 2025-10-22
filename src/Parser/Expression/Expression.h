#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <iostream>

class Token;
class ValueType;

using namespace std;

enum class ExpressionKind {
    NONE,
    LITERAL,
    COMPOSITE_LITERAL,
    GROUPING,
    UNARY,
    BINARY,
    IF_ELSE,
    VARIABLE,
    CALL,
    BLOCK,
    CHAINED,
    CAST
};

class Expression {
private:
    ExpressionKind kind;

protected:
    shared_ptr<ValueType> valueType;

public:
    static shared_ptr<Expression> NONE;

    Expression(ExpressionKind kind, shared_ptr<ValueType> valueType);
    virtual ~Expression() { }
    ExpressionKind getKind();
    shared_ptr<ValueType> getValueType();
};

#endif