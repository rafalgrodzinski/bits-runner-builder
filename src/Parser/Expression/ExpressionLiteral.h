#ifndef EXPRESSION_LITERAL_H
#define EXPRESSION_LITERAL_H

#include "Expression.h"

enum class LiteralKind {
    BOOL,
    UINT,
    SINT,
    FLOAT
};

class ExpressionLiteral: public Expression {
private:
    LiteralKind literalKind;
    bool boolValue;
    uint64_t uIntValue;
    int64_t sIntValue;
    double floatValue;

    static optional<int> charStringToInt(string charString);

public:
    static shared_ptr<ExpressionLiteral> expressionLiteralForToken(shared_ptr<Token> token);
    static shared_ptr<ExpressionLiteral> expressionLiteralForUInt(uint64_t value);
    ExpressionLiteral();
    
    LiteralKind getLiteralKind();
    bool getBoolValue();
    uint64_t getUIntValue();
    int64_t getSIntValue();
    double getFloatValue();
};

#endif