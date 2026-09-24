#ifndef EXPRESSION_BLOCK_H
#define EXPRESSION_BLOCK_H

#include "Expression.h"

class Statement;
class StatementBlock;
class StatementExpression;

class ExpressionBlock: public Expression {
public:
    ExpressionBlock(const vector<shared_ptr<Statement>> &statements, shared_ptr<Location> location);

    shared_ptr<StatementBlock> getStatementBlock() const;
    shared_ptr<StatementExpression> getResultStatementExpression() const;

private:
    shared_ptr<StatementBlock> statementBlock;
    shared_ptr<StatementExpression> resultStatementExpression;
};

#endif