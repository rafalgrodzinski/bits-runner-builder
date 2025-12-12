#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <iostream>
#include <memory>
#include <optional>
#include <vector>

class Token;
class ValueType;

using namespace std;

enum class ExpressionKind {
    BINARY,
    BLOCK,
    CALL,
    CAST,
    CHAINED,
    COMPOSITE_LITERAL,
    GROUPING,
    IF_ELSE,
    LITERAL,
    NONE,
    UNARY,
    VALUE
};

class Expression {
    friend class TypesAnalyzer;

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
    int getLine();
    int getColumn();
    shared_ptr<ValueType> getValueType();
};

#endif