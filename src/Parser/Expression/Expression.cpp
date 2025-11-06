#include "Expression.h"
#include "Parser/ValueType.h"

shared_ptr<Expression> Expression::NONE = make_shared<Expression>(ExpressionKind::NONE, ValueType::NONE, 0, 0);

Expression::Expression(ExpressionKind kind, shared_ptr<ValueType> valueType, int line, int column):
kind(kind), valueType(valueType), line(line), column(column) { }

ExpressionKind Expression::getKind() {
    return kind;
}

shared_ptr<ValueType> Expression::getValueType() {
    return valueType;
}

int Expression::getLine() {
    return line;
}

int Expression::getColumn() {
    return column;
}
