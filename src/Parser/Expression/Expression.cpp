#include "Expression.h"

Expression::Expression(ExpressionKind kind, shared_ptr<ValueType> valueType):
kind(kind), valueType(valueType) { }

ExpressionKind Expression::getKind() {
    return kind;
}

shared_ptr<ValueType> Expression::getValueType() {
    return valueType;
}
