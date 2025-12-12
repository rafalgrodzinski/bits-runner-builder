#ifndef STATEMENT_BLOCK
#define STATEMENT_BLOCK

#include "Parser/Statement/Statement.h"

class StatementExpression;

class StatementBlock: public Statement {
private:
    vector<shared_ptr<Statement>> statements;

public:
    StatementBlock(vector<shared_ptr<Statement>> statements, int line, int column);
    vector<shared_ptr<Statement>> getStatements();
};

#endif