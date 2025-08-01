#ifndef STATEMENT_TYPE_H
#define STATEMENT_TYPE_H

#include "Statement.h"

class StatementVariable;

class StatementType: public Statement {
private:
    string identifier;
    vector<shared_ptr<StatementVariable>> statementVariables;

public:
    StatementType(string identifier, vector<shared_ptr<StatementVariable>> statementVariables);
    string getIdentifier();
    vector<shared_ptr<StatementVariable>> getStatementVariables();
};

#endif