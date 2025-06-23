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
    BLOCK,
    INVALID
};

class Expression {
private:
    ExpressionKind kind;

protected:
    ValueType valueType;

public:
    Expression(ExpressionKind kind, ValueType valueType);
    ExpressionKind getKind();
    ValueType getValueType();
    bool isValid();
    virtual string toString(int indent);
};

#endif