#ifndef STATEMENT_MODULE_H
#define STATEMENT_MODULE_H

#include "Statement.h"

class StatementModule: public Statement {
private:
    string name;
    vector<shared_ptr<Statement>> statements;
    vector<shared_ptr<Statement>> headerStatements;
    vector<shared_ptr<Statement>> exportedHeaderStatements;

public:
    StatementModule(string name, vector<shared_ptr<Statement>> statements, vector<shared_ptr<Statement>> headerStatements, vector<shared_ptr<Statement>> exportedHeaderStatements);
    string getName();
    vector<shared_ptr<Statement>> getStatements();
    vector<shared_ptr<Statement>> getHeaderStatements();
    vector<shared_ptr<Statement>> getExportedHeaderStatements();
};

#endif