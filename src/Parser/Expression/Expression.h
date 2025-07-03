#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <iostream>

#include "Lexer/Token.h"
#include "Types.h"

using namespace std;

enum class ExpressionKind {
    LITERAL,
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
    ValueType valueType;

public:
    Expression(ExpressionKind kind, ValueType valueType);
    virtual ~Expression() { }
    ExpressionKind getKind();
    ValueType getValueType();
};

#endif