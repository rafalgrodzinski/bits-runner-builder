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
    VALUE,
    CALL,
    BLOCK,
    CHAINED,
    CAST
};

class Expression {
private:
    ExpressionKind kind;
    int line;
    int column;

protected:
    shared_ptr<ValueType> valueType;

public:
    static shared_ptr<Expression> NONE;

    Expression(ExpressionKind kind, shared_ptr<ValueType> valueType, int line, int column);
    virtual ~Expression() { }
    ExpressionKind getKind();
    shared_ptr<ValueType> getValueType();
    int getLine();
    int getColumn();

    friend class TypesAnalyzer;
};

#endif