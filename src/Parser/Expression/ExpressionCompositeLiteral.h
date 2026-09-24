#ifndef EXPRESSION_COMPOSITE_LITERAL_H
#define EXPRESSION_COMPOSITE_LITERAL_H

#include "Expression.h"

class ExpressionCompositeLiteral: public Expression {
friend class Analyzer;

public:
    static std::shared_ptr<ExpressionCompositeLiteral> expressionCompositeLiteralForExpressions(std::vector<std::shared_ptr<Expression>> expressions, std::shared_ptr<Location> location);
    static std::shared_ptr<ExpressionCompositeLiteral> expressionCompositeLiteralForTokenString(std::shared_ptr<Token> tokenString);

    ExpressionCompositeLiteral(std::shared_ptr<Location> location);

    std::vector<std::shared_ptr<Expression>> getExpressions() const;

private:
    std::vector<std::shared_ptr<Expression>> expressions;
};

#endif