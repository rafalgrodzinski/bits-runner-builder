#ifndef STATEMENT_TYPE_H
#define STATEMENT_TYPE_H

#include "Statement.h"

class ValueType;

class StatementBlob: public Statement {
private:
    bool shouldExport;
    string name;
    vector<pair<string, shared_ptr<ValueType>>> members;

public:
    StatementBlob(bool shouldExport, string name, vector<pair<string, shared_ptr<ValueType>>> members, shared_ptr<Location> location);
    bool getShouldExport();
    string getName();
    vector<pair<string, shared_ptr<ValueType>>> getMembers();
};

#endif