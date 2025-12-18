#ifndef STATEMENT_BLOCK
#define STATEMENT_BLOCK

#include "Parser/Statement/Statement.h"

class StatementExpression;

class StatementBlock: public Statement {
private:
    vector<shared_ptr<Statement>> statements;

public:
    StatementBlock(vector<shared_ptr<Statement>> statements, shared_ptr<Location> location);
    vector<shared_ptr<Statement>> getStatements();
};

#endif