#include "Expression.h"

Expression::Expression(ExpressionKind kind, ValueType valueType):
    kind(kind), valueType(valueType) {
}

ExpressionKind Expression::getKind() {
    return kind;
}

ValueType Expression::getValueType() {
    return valueType;
}
