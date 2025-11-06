#ifndef STATEMENT_META_EXTERN_VARIABLE_H
#define STATEMENT_META_EXTERN_VARIABLE_H

#include "Statement.h"

class ValueType;

class StatementMetaExternVariable: public Statement {
private:
    string identifier;
    shared_ptr<ValueType> valueType;

public:
    StatementMetaExternVariable(string identifier, shared_ptr<ValueType> valueType, int line, int column);
    string getIdentifier();
    shared_ptr<ValueType> getValueType();
};

#endif