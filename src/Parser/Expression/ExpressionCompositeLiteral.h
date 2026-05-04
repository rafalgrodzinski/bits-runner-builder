#ifndef EXPRESSION_COMPOSITE_LITERAL_H
#define EXPRESSION_COMPOSITE_LITERAL_H

#include "Expression.h"

class ExpressionCompositeLiteral: public Expression {
friend class Analyzer;

private:
    vector<shared_ptr<Expression>> expressions;
    
public:
    static shared_ptr<ExpressionCompositeLiteral> expressionCompositeLiteralForExpressions(vector<shared_ptr<Expression>> expressions, shared_ptr<Location> location);
    static shared_ptr<ExpressionCompositeLiteral> expressionCompositeLiteralForTokenString(shared_ptr<Token> tokenString);

    ExpressionCompositeLiteral(shared_ptr<Location> location);
    vector<shared_ptr<Expression>> getExpressions();
};

#endif