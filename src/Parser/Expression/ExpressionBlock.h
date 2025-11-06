#ifndef EXPRESSION_BLOCK_H
#define EXPRESSION_BLOCK_H

#include "Expression.h"

class Statement;
class StatementBlock;
class StatementExpression;

class ExpressionBlock: public Expression {
private:
    shared_ptr<StatementBlock> statementBlock;
    shared_ptr<StatementExpression> resultStatementExpression;

public:
    ExpressionBlock(vector<shared_ptr<Statement>> statements, int line, int column);
    shared_ptr<StatementBlock> getStatementBlock();
    shared_ptr<StatementExpression> getResultStatementExpression();
};

#endif