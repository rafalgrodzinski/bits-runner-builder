#ifndef EXPRESSION_LITERAL_H
#define EXPRESSION_LITERAL_H

#include "Expression.h"

enum class ExpressionLiteralKind {
    BOOL,
    UINT,
    FLOAT
};

class ExpressionLiteral: public Expression {
private:
    ExpressionLiteralKind literalKind;
    bool boolValue;
    uint64_t uIntValue;
    double floatValue;

    static optional<int> decodeEscapedCharString(string charString);

public:
    static shared_ptr<ExpressionLiteral> expressionLiteralForToken(shared_ptr<Token> token);
    static shared_ptr<ExpressionLiteral> expressionLiteralForUInt(uint64_t value, shared_ptr<Location> location);
    ExpressionLiteral(shared_ptr<Location> location);
    
    ExpressionLiteralKind getLiteralKind();
    bool getBoolValue();
    uint64_t getUIntValue();
    double getFloatValue();
};

#endif