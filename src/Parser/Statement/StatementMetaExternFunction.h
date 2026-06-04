#ifndef STATEMENT_META_EXTERN_FUNCTION_H
#define STATEMENT_META_EXTERN_FUNCTION_H

#include "Statement.h"

class ValueType;

class StatementMetaExternFunction: public Statement {
private:
    string name;
    vector<pair<string, shared_ptr<ValueType>>> arguments;
    shared_ptr<ValueType> returnValueType;

public:
    StatementMetaExternFunction(const string &name, const vector<pair<string, shared_ptr<ValueType>>> &arguments, shared_ptr<ValueType> returnValueType, shared_ptr<Location> location);
    string getName() const;
    vector<pair<string, shared_ptr<ValueType>>> getArguments() const;
    shared_ptr<ValueType> getReturnValueType() const;
    shared_ptr<ValueType> getValueType() const;
};

#endif