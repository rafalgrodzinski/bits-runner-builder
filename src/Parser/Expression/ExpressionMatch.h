#ifndef EXPRESSION_MATCH
#define EXPRESSION_MATCH

#include "Expression.h"

class ExpressionMatch: public Expression {
public:
    ExpressionMatch(
        std::shared_ptr<Expression> switchExpression,
        std::vector<std::pair<std::shared_ptr<Expression>, std::shared_ptr<Expression>>> casePairs,
        std::shared_ptr<Expression> elseExpression,
        std::shared_ptr<Location> location
    );

    std::shared_ptr<Expression> getSwitchExpression();
    std::vector<std::pair<std::shared_ptr<Expression>, std::shared_ptr<Expression>>> getCasePairs();
    std::shared_ptr<Expression> getElseExpression();

private:
    std::shared_ptr<Expression> switchExpression;
    std::vector<std::pair<std::shared_ptr<Expression>, std::shared_ptr<Expression>>> casePairs;
    std::shared_ptr<Expression> elseExpression;
};

#endif