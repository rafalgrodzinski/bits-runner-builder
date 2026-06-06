#include "Expression.h"

#include "Lexer/Location.h"
#include "Parser/ValueType.h"

shared_ptr<Expression> Expression::NONE = make_shared<Expression>(ExpressionKind::NONE, ValueType::NONE, nullptr);

Expression::Expression(ExpressionKind kind, shared_ptr<ValueType> valueType, shared_ptr<Location> location):
kind(kind), valueType(valueType), location(location) { }

ExpressionKind Expression::getKind() const {
    return kind;
}

shared_ptr<Location> Expression::getLocation() const {
    return location;
}

shared_ptr<ValueType> Expression::getValueType() const {
    return valueType;
}
