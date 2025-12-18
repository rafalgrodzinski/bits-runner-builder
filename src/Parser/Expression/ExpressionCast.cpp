#include "ExpressionCast.h"

ExpressionCast::ExpressionCast(shared_ptr<ValueType> valueType, shared_ptr<Location> location):
Expression(ExpressionKind::CAST, valueType, location) { }