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
    vector<string> namedTypeKeys;
    vector<shared_ptr<StatementVariable>> variableStatements;
    vector<shared_ptr<StatementFunction>> functionStatements;
    vector<string> protoNames;

public:
    StatementBlob(
        bool shouldExport,
        string name,
        vector<string> namedTypeKeys,
        vector<string> protoNames,
        vector<shared_ptr<StatementVariable>> variableStatements,
        vector<shared_ptr<StatementFunction>> functionStatements,
        shared_ptr<Location> location
    );

    bool getShouldExport() const;
    string getName() const;
    vector<string> getNamedTypeKeys() const;
    vector<string> getProtoNames() const;
    vector<shared_ptr<StatementVariable>> getVariableStatements() const;
    vector<shared_ptr<StatementFunction>> getFunctionStatements() const;
    vector<pair<string, shared_ptr<ValueType>>> getMembers() const;
};

#endif