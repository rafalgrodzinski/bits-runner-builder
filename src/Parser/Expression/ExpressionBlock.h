#include "Parser/Expression/Expression.h"

class Statement;
class StatementExpression;

class ExpressionBlock: public Expression {
private:
    vector<shared_ptr<Statement>> statements;
    shared_ptr<StatementExpression> resultExpression;

public:
    ExpressionBlock(vector<shared_ptr<Statement>> statements);
    string toString(int indent) override;
};