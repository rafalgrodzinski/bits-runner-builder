#ifndef EXPRESSION_LITERAL_H
#define EXPRESSION_LITERAL_H

#include "Expression.h"

class ExpressionLiteral: public Expression {
private:
    bool boolValue;
    int32_t sint32Value;
    float real32Value;

public:
    static shared_ptr<ExpressionLiteral> expressionLiteralForToken(shared_ptr<Token> token);
    
    ExpressionLiteral();
    bool getBoolValue();
    int32_t getSint32Value();
    float getReal32Value();
};

#endif