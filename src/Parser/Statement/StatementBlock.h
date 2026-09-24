#ifndef STATEMENT_BLOCK
#define STATEMENT_BLOCK

#include "Parser/Statement/Statement.h"

class StatementExpression;

class StatementBlock: public Statement {
public:
    StatementBlock(const std::vector<std::shared_ptr<Statement>> &statements, std::shared_ptr<Location> location);

    std::vector<std::shared_ptr<Statement>> getStatements() const;

private:
    std::vector<std::shared_ptr<Statement>> statements;
};

#endif