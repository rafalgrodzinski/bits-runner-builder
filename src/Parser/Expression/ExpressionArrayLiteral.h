#ifndef EXPRESSION_ARRAY_LITERAL_H
#define EXPRESSION_ARRAY_LITERAL_H

#include "Expression.h"

class ExpressionArrayLiteral: public Expression {
private:
    vector<shared_ptr<Expression>> expressions;
    
public:
    static shared_ptr<ExpressionArrayLiteral> expressionArrayLiteralForExpressions(vector<shared_ptr<Expression>> expressions);
    static shared_ptr<ExpressionArrayLiteral> expressionArrayLiteralForTokenString(shared_ptr<Token> tokenString);

    ExpressionArrayLiteral();
    vector<shared_ptr<Expression>> getExpressions();
};

#endif