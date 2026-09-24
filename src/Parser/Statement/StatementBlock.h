#ifndef STATEMENT_BLOCK
#define STATEMENT_BLOCK

#include "Parser/Statement/Statement.h"

class StatementExpression;

class StatementBlock: public Statement {
public:
    StatementBlock(const vector<shared_ptr<Statement>> &statements, shared_ptr<Location> location);

    vector<shared_ptr<Statement>> getStatements() const;

private:
    vector<shared_ptr<Statement>> statements;
};

#endif