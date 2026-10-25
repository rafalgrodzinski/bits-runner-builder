#ifndef EXPRESSION_CAST_H
#define EXPRESSION_CAST_H

#include "Expression.h"

class ExpressionCast: public Expression {
public:
    ExpressionCast(std::shared_ptr<ValueType> valueType, bool shouldClamp, std::shared_ptr<Location> location);
    bool getShouldClamp() const;

private:
    bool shouldClamp;
};

#endif