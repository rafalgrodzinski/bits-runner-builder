#include "ExpressionCast.h"

ExpressionCast::ExpressionCast(shared_ptr<ValueType> valueType, int line, int column):
Expression(ExpressionKind::CAST, valueType, line, column) { }