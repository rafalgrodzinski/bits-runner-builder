#include "Expression.h"

ExpressionInvalid Expression::Invalid = ExpressionInvalid();

std::string ExpressionInvalid::toString() {
    return "Invalid";
}

/*ExpressionInteger::ExpressionInteger(Token token) {
}

std::string ExpressionInteger::toString() {
    return "INTEGER";
}*/