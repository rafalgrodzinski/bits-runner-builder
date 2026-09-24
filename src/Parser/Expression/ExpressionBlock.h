#ifndef EXPRESSION_BLOCK_H
#define EXPRESSION_BLOCK_H

#include "Expression.h"

class Statement;
class StatementBlock;
class StatementExpression;

class ExpressionBlock: public Expression {
public:
    ExpressionBlock(const std::vector<std::shared_ptr<Statement>> &statements, std::shared_ptr<Location> location);

    std::shared_ptr<StatementBlock> getStatementBlock() const;
    std::shared_ptr<StatementExpression> getResultStatementExpression() const;

private:
    std::shared_ptr<StatementBlock> statementBlock;
    std::shared_ptr<StatementExpression> resultStatementExpression;
};

#endif