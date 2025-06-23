#include "Parser/Expression.h"

class ExpressionBlock: public Expression {
private:
    vector<shared_ptr<Statement>> statements;
    shared_ptr<StatementExpression> resultExpression;

public:
    ExpressionBlock(vector<shared_ptr<Statement>> statements);
    string toString(int indent) override;
};