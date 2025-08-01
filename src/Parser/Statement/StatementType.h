#ifndef STATEMENT_TYPE_H
#define STATEMENT_TYPE_H

#include "Statement.h"

class StatementVariable;
class ValueType;

class StatementType: public Statement {
private:
    string identifier;
    vector<pair<string, shared_ptr<ValueType>>> variables;

public:
    StatementType(string identifier, vector<pair<string, shared_ptr<ValueType>>> variables);
    string getIdentifier();
    vector<pair<string, shared_ptr<ValueType>>> getVariables();
};

#endif