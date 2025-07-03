#include "Parser/Statement/Statement.h"

class StatementExpression;

class StatementBlock: public Statement {
private:
    vector<shared_ptr<Statement>> statements;

public:
    StatementBlock(vector<shared_ptr<Statement>> statements);
    vector<shared_ptr<Statement>> getStatements();
};