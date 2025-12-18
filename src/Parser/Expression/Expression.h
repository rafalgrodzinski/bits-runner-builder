#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <iostream>
#include <memory>
#include <optional>
#include <vector>

class Location;
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
    friend class Analyzer;

private:
    ExpressionKind kind;
    shared_ptr<Location> location;

protected:
    shared_ptr<ValueType> valueType;

public:
    static shared_ptr<Expression> NONE;

    Expression(ExpressionKind kind, shared_ptr<ValueType> valueType, shared_ptr<Location> location);
    virtual ~Expression() { }
    ExpressionKind getKind();
    shared_ptr<Location> getLocation();
    int getLine();
    int getColumn();
    shared_ptr<ValueType> getValueType();
};

#endif