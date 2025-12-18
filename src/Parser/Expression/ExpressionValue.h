#ifndef EXPRESSION_VALUE_H
#define EXPRESSION_VALUE_H

#include "Expression.h"

enum class ExpressionValueKind {
    BUILT_IN_COUNT,
    BUILT_IN_ADR,
    BUILT_IN_SIZE,
    BUILT_IN_VADR,
    BUILT_IN_VAL_SIMPLE,
    BUILT_IN_VAL_DATA,
    DATA,
    FUN,
    SIMPLE
};

class ExpressionValue: public Expression {
    friend class Analyzer;

private:
    ExpressionValueKind valueKind;
    string identifier;
    shared_ptr<Expression> indexExpression;

public:
    static shared_ptr<ExpressionValue> simple(string identifer, int line, int column);
    static shared_ptr<ExpressionValue> data(string identifier, shared_ptr<Expression> indexExpression, int line, int column);

    ExpressionValue(int line, int column);
    ExpressionValueKind getValueKind();
    string getIdentifier();
    shared_ptr<Expression> getIndexExpression();
};

#endif