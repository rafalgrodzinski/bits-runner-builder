#ifndef EXPRESSION_LITERAL_H
#define EXPRESSION_LITERAL_H

#include "Expression.h"

enum class LiteralKind {
    BOOL,
    UINT,
    SINT,
    REAL
};


class ExpressionLiteral: public Expression {
private:
    LiteralKind literalKind;
    bool boolValue;
    uint64_t uIntValue;
    int64_t sIntValue;
    double realValue;

public:
    static shared_ptr<ExpressionLiteral> expressionLiteralForToken(shared_ptr<Token> token);
    ExpressionLiteral();
    
    LiteralKind getLiteralKind();
    bool getBoolValue();
    uint64_t getUIntValue();
    int64_t getSIntValue();
    double getRealValue();
};

#endif