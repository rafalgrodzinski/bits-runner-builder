#include "Expression.h"

#include "Lexer/Location.h"
#include "Parser/ValueType.h"

shared_ptr<Expression> Expression::NONE = make_shared<Expression>(ExpressionKind::NONE, ValueType::NONE, make_shared<Location>("", 0, 0));

Expression::Expression(ExpressionKind kind, shared_ptr<ValueType> valueType, shared_ptr<Location> location):
kind(kind), valueType(valueType), location(location) { }

ExpressionKind Expression::getKind() {
    return kind;
}

shared_ptr<Location> Expression::getLocation() {
    return location;
}

int Expression::getLine() {
    return location->getLine();
}

int Expression::getColumn() {
    return location->getColumn();
}

shared_ptr<ValueType> Expression::getValueType() {
    return valueType;
}
