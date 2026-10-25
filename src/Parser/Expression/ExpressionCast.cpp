#include "ExpressionCast.h"

using namespace std;

// MARK: - Public

ExpressionCast::ExpressionCast(shared_ptr<ValueType> valueType, bool shouldClamp, shared_ptr<Location> location):
Expression(ExpressionKind::CAST, valueType, location),
shouldClamp(shouldClamp) { }

bool ExpressionCast::getShouldClamp() const {
    return shouldClamp;
}