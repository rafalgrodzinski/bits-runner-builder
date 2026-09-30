#ifndef EXPRESSION_MATCH
#define EXPRESSION_MATCH

#include "Expression.h"

class ExpressionMatch: public Expression {
public:
    ExpressionMatch(
        std::shared_ptr<Expression> switchExpression,
        std::shared_ptr<Expression> elseExpression,
        std::shared_ptr<Location> location
    );

    std::shared_ptr<Expression> getSwitchExpression();
    std::shared_ptr<Expression> getElseExpression();

private:
    std::shared_ptr<Expression> switchExpression;
    std::shared_ptr<Expression> elseExpression;
};

#endif