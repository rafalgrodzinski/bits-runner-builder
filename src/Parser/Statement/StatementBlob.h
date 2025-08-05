#ifndef STATEMENT_TYPE_H
#define STATEMENT_TYPE_H

#include "Statement.h"

class ValueType;

class StatementBlob: public Statement {
private:
    string identifier;
    vector<pair<string, shared_ptr<ValueType>>> variables;

public:
    StatementBlob(string identifier, vector<pair<string, shared_ptr<ValueType>>> variables);
    string getIdentifier();
    vector<pair<string, shared_ptr<ValueType>>> getVariables();
};

#endif