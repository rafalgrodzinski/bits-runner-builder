#ifndef EXPRESSION_CAST_H
#define EXPRESSION_CAST_H

#include "Expression.h"

class ExpressionCast: public Expression {
public:
    ExpressionCast(std::shared_ptr<ValueType> valueType, std::shared_ptr<Location> location);
};

#endif