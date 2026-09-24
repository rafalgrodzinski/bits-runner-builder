#include "Expression.h"

#include "Lexer/Location.h"
#include "Parser/ValueType/ValueTypeSimple.h"

using namespace std;

// MARK: - Public

shared_ptr<Expression> Expression::none(shared_ptr<Location> location) {
    return make_shared<Expression>(ExpressionKind::NONE, ValueTypeSimple::NONE, location);
}

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
