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
    StatementMetaExternFunction(string name, vector<pair<string, shared_ptr<ValueType>>> arguments, shared_ptr<ValueType> returnValueType);
    string getName();
    vector<pair<string, shared_ptr<ValueType>>> getArguments();
    shared_ptr<ValueType> getReturnValueType();
};

#endif