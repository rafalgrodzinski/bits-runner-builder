#ifndef EXPRESSION_CAST_H
#define EXPRESSION_CAST_H

#include "Expression.h"

class ExpressionCast: public Expression {
public:
    ExpressionCast(shared_ptr<ValueType> valueType, shared_ptr<Location> location);
};

#endif