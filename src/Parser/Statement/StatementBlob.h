#ifndef STATEMENT_TYPE_H
#define STATEMENT_TYPE_H

#include "Statement.h"

class ValueType;

class StatementBlob: public Statement {
private:
    string name;
    vector<pair<string, shared_ptr<ValueType>>> members;

public:
    StatementBlob(string name, vector<pair<string, shared_ptr<ValueType>>> members);
    string getName();
    vector<pair<string, shared_ptr<ValueType>>> getMembers();
};

#endif