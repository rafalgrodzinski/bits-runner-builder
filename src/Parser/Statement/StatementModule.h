#ifndef STATEMENT_MODULE_H
#define STATEMENT_MODULE_H

#include "Statement.h"

class StatementModule: public Statement {
private:
    string name;
    vector<shared_ptr<Statement>> statements;

public:
    StatementModule(string name, vector<shared_ptr<Statement>> statements);
    string getName();
    vector<shared_ptr<Statement>> getStatements();
};

#endif