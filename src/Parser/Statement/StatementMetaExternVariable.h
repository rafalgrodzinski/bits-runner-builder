#ifndef STATEMENT_META_EXTERN_VARIABLE_H
#define STATEMENT_META_EXTERN_VARIABLE_H

#include "Statement.h"

class ValueType;

class StatementMetaExternVariable: public Statement {
private:
    string name;
    shared_ptr<ValueType> valueType;

public:
    StatementMetaExternVariable(string name, shared_ptr<ValueType> valueType);
    string getName();
    shared_ptr<ValueType> getValueType();
};

#endif