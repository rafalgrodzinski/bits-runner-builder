#include "Parser/Expression/Expression.h"

class Statement;
class StatementBlock;
class StatementExpression;

class ExpressionBlock: public Expression {
private:
    shared_ptr<StatementBlock> statementBlock;
    shared_ptr<StatementExpression> resultStatementExpression;

public:
    ExpressionBlock(vector<shared_ptr<Statement>> statements);
    shared_ptr<StatementBlock> getStatementBlock();
    shared_ptr<StatementExpression> getResultStatementExpression();
    string toString(int indent) override;
};