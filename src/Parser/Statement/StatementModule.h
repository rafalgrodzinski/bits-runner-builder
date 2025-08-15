#ifndef STATEMENT_MODULE_H
#define STATEMENT_MODULE_H

#include "Statement.h"

class StatementModule: public Statement {
private:
    string name;
    vector<shared_ptr<Statement>> statements;
    vector<shared_ptr<Statement>> headerStatements;

public:
    StatementModule(string name, vector<shared_ptr<Statement>> statements, vector<shared_ptr<Statement>> headerStatements);
    string getName();
    vector<shared_ptr<Statement>> getStatements();
    vector<shared_ptr<Statement>> getHeaderStatements();
};

#endif