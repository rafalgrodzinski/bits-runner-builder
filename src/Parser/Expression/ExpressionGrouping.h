#include "Parser/Expression/Expression.h"

class ExpressionGrouping: public Expression {
private:
    shared_ptr<Expression> expression;

public:
    ExpressionGrouping(shared_ptr<Expression> expression);
    shared_ptr<Expression> getExpression();
    string toString(int indent) override;
};