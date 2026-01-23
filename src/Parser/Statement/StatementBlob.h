#ifndef STATEMENT_TYPE_H
#define STATEMENT_TYPE_H

#include "Statement.h"

class StatementFunction;
class StatementVariable;

class ValueType;

class StatementBlob: public Statement {
private:
    bool shouldExport;
    string name;
    vector<shared_ptr<StatementVariable>> variableStatements;
    vector<shared_ptr<StatementFunction>> functionStatements;

public:
    StatementBlob(
        bool shouldExport,
        string name,
        vector<shared_ptr<StatementVariable>> variableStatements,
        vector<shared_ptr<StatementFunction>> functionStatements,
        shared_ptr<Location> location
    );
    bool getShouldExport();
    string getName();
    vector<shared_ptr<StatementVariable>> getVariableStatements();
    vector<shared_ptr<StatementFunction>> getFunctionStatements();
    vector<pair<string, shared_ptr<ValueType>>> getMembers();
};

#endif