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

public:
    static shared_ptr<Expression> none(shared_ptr<Location> location);

    Expression(ExpressionKind kind, shared_ptr<ValueType> valueType, shared_ptr<Location> location);
    virtual ~Expression() { }

    ExpressionKind getKind() const;
    shared_ptr<Location> getLocation() const;
    shared_ptr<ValueType> getValueType() const;

protected:
    shared_ptr<ValueType> valueType;

private:
    ExpressionKind kind;
    shared_ptr<Location> location;
};

#endif