#include "ExpressionCast.h"

using namespace std;

// MARK: - Public

ExpressionCast::ExpressionCast(shared_ptr<ValueType> valueType, shared_ptr<Location> location):
Expression(ExpressionKind::CAST, valueType, location) { }