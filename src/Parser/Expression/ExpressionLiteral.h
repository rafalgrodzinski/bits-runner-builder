#ifndef EXPRESSION_LITERAL_H
#define EXPRESSION_LITERAL_H

#include "Expression.h"

enum class ExpressionLiteralKind {
    BOOL,
    UINT,
    SINT,
    FLOAT
};

class ExpressionLiteral: public Expression {
friend class Analyzer;

private:
    ExpressionLiteralKind literalKind;
    bool boolValue;
    uint64_t uIntValue;
    int64_t sIntValue;
    double floatValue;

    static optional<int> decodeEscapedCharString(string charString);

public:
    static shared_ptr<ExpressionLiteral> expressionLiteralForToken(shared_ptr<Token> token);
    static shared_ptr<ExpressionLiteral> expressionLiteralForInt(int64_t value, shared_ptr<Location> location);
    ExpressionLiteral(shared_ptr<Location> location);
    
    ExpressionLiteralKind getLiteralKind();
    bool getBoolValue();
    uint64_t getUIntValue();
    int64_t getSIntValue();
    double getFloatValue();
};

#endif