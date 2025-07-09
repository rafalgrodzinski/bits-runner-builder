#include "Parser/Expression/Expression.h"

class ExpressionVariable: public Expression {
private:
    string name;
    shared_ptr<Expression> indexExpression;

public:
    ExpressionVariable(string name, shared_ptr<Expression> indexExpression);
    string getName();
    shared_ptr<Expression> getIndexExpression();
};