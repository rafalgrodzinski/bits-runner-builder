#ifndef EXPRESSION_VALUE_H
#define EXPRESSION_VALUE_H

#include "Expression.h"

enum class ExpressionValueKind {
    SIMPLE,
    DATA
};

class ExpressionValue: public Expression {
private:
    ExpressionValueKind variableKind;
    string identifier;
    shared_ptr<Expression> indexExpression;

public:
    static shared_ptr<ExpressionValue> simple(string identifer, int line, int column);
    static shared_ptr<ExpressionValue> data(string identifier, shared_ptr<Expression> indexExpression, int line, int column);

    ExpressionValue(int line, int column);
    ExpressionValueKind getVariableKind();
    string getIdentifier();
    shared_ptr<Expression> getIndexExpression();
};

#endif