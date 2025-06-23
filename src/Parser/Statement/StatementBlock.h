#include "Parser/Statement/Statement.h"

class StatementBlock: public Statement {
private:
    vector<shared_ptr<Statement>> statements;
    shared_ptr<StatementExpression> statementExpression;

public:
    StatementBlock(vector<shared_ptr<Statement>> statements);
    vector<shared_ptr<Statement>> getStatements();
    shared_ptr<StatementExpression> getStatementExpression();
    string toString(int indent) override;
};