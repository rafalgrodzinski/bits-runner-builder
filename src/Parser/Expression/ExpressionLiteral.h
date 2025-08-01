#ifndef EXPRESSION_LITERAL_H
#define EXPRESSION_LITERAL_H

#include "Expression.h"

class ExpressionLiteral: public Expression {
private:
    bool boolValue;
    uint8_t u8Value;
    uint32_t u32Value;
    int8_t s8Value;
    int32_t s32Value;
    float r32Value;

public:
    static shared_ptr<ExpressionLiteral> expressionLiteralForToken(shared_ptr<Token> token);
    
    ExpressionLiteral();
    bool getBoolValue();
    uint8_t getU8Value();
    uint32_t getU32Value();
    int8_t getS8Value();
    int32_t getS32Value();
    float getR32Value();
};

#endif