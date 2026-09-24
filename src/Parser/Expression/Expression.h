#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <iostream>
#include <memory>
#include <optional>
#include <vector>

class Location;
class Token;
class ValueType;

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
    static std::shared_ptr<Expression> none(std::shared_ptr<Location> location);

    Expression(ExpressionKind kind, std::shared_ptr<ValueType> valueType, std::shared_ptr<Location> location);
    virtual ~Expression() { }

    ExpressionKind getKind() const;
    std::shared_ptr<Location> getLocation() const;
    std::shared_ptr<ValueType> getValueType() const;

protected:
    std::shared_ptr<ValueType> valueType;

private:
    ExpressionKind kind;
    std::shared_ptr<Location> location;
};

#endif