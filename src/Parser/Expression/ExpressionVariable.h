#ifndef EXPRESSION_VARIABLE_H
#define EXPRESSION_VARIABLE_H

#include "Expression.h"

enum class ExpressionVariableKind {
    SIMPLE,
    DATA
};

class ExpressionVariable: public Expression {
private:
    ExpressionVariableKind variableKind;
    string identifier;
    shared_ptr<Expression> indexExpression;

public:
    static shared_ptr<ExpressionVariable> simple(string identifer);
    static shared_ptr<ExpressionVariable> data(string identifier, shared_ptr<Expression> indexExpression);

    ExpressionVariable();
    ExpressionVariableKind getVariableKind();
    string getIdentifier();
    shared_ptr<Expression> getIndexExpression();
};

#endif