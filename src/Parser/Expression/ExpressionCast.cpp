#include "ExpressionCast.h"

ExpressionCast::ExpressionCast(shared_ptr<ValueType> valueType):
Expression(ExpressionKind::CAST, valueType) { }