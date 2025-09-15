#ifndef EXPRESSION_COMPOSITE_LITERAL_H
#define EXPRESSION_COMPOSITE_LITERAL_H

#include "Expression.h"

class ExpressionCompositeLiteral: public Expression {
private:
    vector<shared_ptr<Expression>> expressions;
    
public:
    static shared_ptr<ExpressionCompositeLiteral> expressionCompositeLiteralForExpressions(vector<shared_ptr<Expression>> expressions);

    ExpressionCompositeLiteral();
    vector<shared_ptr<Expression>> getExpressions();
};

#endif