#ifndef EXPRESSION_LITERAL_H
#define EXPRESSION_LITERAL_H

#include "Expression.h"

class ExpressionLiteral: public Expression {
private:
    bool boolValue;
    uint8_t u8Value;
    uint32_t u32Value;
    uint64_t u64Value;
    int8_t s8Value;
    int32_t s32Value;
    int64_t s64Value;
    float r32Value;

public:
    static shared_ptr<ExpressionLiteral> expressionLiteralForToken(shared_ptr<Token> token);
    
    ExpressionLiteral();
    bool getBoolValue();
    uint8_t getU8Value();
    uint32_t getU32Value();
    uint64_t getU64Value();
    int8_t getS8Value();
    int32_t getS32Value();
    int64_t getS64Value();
    float getR32Value();
};

#endif