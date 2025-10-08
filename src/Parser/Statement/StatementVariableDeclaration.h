#ifndef STATEMENT_VARIABLE_DECLARATION_H
#define STATEMENT_VARIABLE_DECLARATION_H

#include "Statement.h"

class ValueType;

class StatementVariableDeclaration: public Statement {
private:
    bool shouldExport;
    string name;
    shared_ptr<ValueType> valueType;

public:
    StatementVariableDeclaration(bool shouldExport, string name, shared_ptr<ValueType> valueType);
    bool getShouldExport();
    string getName();
    shared_ptr<ValueType> getValueType();
};

#endif