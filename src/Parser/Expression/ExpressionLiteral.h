#ifndef EXPRESSION_LITERAL_H
#define EXPRESSION_LITERAL_H

#include "Expression.h"

enum class ExpressionLiteralKind {
    BOOL,
    UINT,
    FLOAT
};

class ExpressionLiteral: public Expression {
public:
    static std::shared_ptr<ExpressionLiteral> expressionLiteralForToken(std::shared_ptr<Token> token);
    static std::shared_ptr<ExpressionLiteral> expressionLiteralForUInt(uint64_t value, std::shared_ptr<Location> location);

    ExpressionLiteral(std::shared_ptr<Location> location);
    
    ExpressionLiteralKind getLiteralKind() const;
    bool getBoolValue() const;
    uint64_t getUIntValue() const;
    double getFloatValue() const;

private:
    ExpressionLiteralKind literalKind;
    bool boolValue;
    uint64_t uIntValue;
    double floatValue;

    static std::optional<int> decodeEscapedCharString(std::string charString);
};

#endif