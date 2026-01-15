#ifndef STATEMENT_TYPE_H
#define STATEMENT_TYPE_H

#include "Statement.h"

class ValueType;

class StatementBlob: public Statement {
private:
    bool shouldExport;
    string name;
    vector<shared_ptr<Statement>> variableStatements;
    vector<shared_ptr<Statement>> functionStatements;

public:
    StatementBlob(
        bool shouldExport,
        string name,
        vector<shared_ptr<Statement>> variableStatements,
        vector<shared_ptr<Statement>> functionStatements,
        shared_ptr<Location> location
    );
    bool getShouldExport();
    string getName();
    vector<shared_ptr<Statement>> getVariableStatements();
    vector<shared_ptr<Statement>> getFunctionStatements();
};

#endif