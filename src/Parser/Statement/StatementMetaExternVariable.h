#ifndef STATEMENT_META_EXTERN_VARIABLE_H
#define STATEMENT_META_EXTERN_VARIABLE_H

#include "Statement.h"

class ValueType;

class StatementMetaExternVariable: public Statement {
private:
    string identifier;
    shared_ptr<ValueType> valueType;

public:
    StatementMetaExternVariable(const string &identifier, shared_ptr<ValueType> valueType, shared_ptr<Location> location);
    string getIdentifier() const;
    shared_ptr<ValueType> getValueType() const;
};

#endif