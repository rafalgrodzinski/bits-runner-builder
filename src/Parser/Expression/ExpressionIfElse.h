#include "Parser/Expression/Expression.h"

class ExpressionBlock;

class ExpressionIfElse: public Expression {
private:
    shared_ptr<Expression> condition;
    shared_ptr<ExpressionBlock> thenBlock;
    shared_ptr<ExpressionBlock> elseBlock;

public:
    ExpressionIfElse(shared_ptr<Expression> condition, shared_ptr<ExpressionBlock> thenBlock, shared_ptr<ExpressionBlock> elseBlock);
    shared_ptr<Expression> getCondition();
    shared_ptr<ExpressionBlock> getThenBlock();
    shared_ptr<ExpressionBlock> getElseBlock();
    string toString(int indent) override;
};