#ifndef STATEMENT_VARIABLE_DECLARATION_H
#define STATEMENT_VARIABLE_DECLARATION_H

#include "Statement.h"

class ValueType;

class StatementVariableDeclaration: public Statement {
friend class TypesAnalyzer;

private:
    bool shouldExport;
    string identifier;
    shared_ptr<ValueType> valueType;

public:
    StatementVariableDeclaration(bool shouldExport, string identifier, shared_ptr<ValueType> valueType, int line, int column);
    bool getShouldExport();
    string getIdentifier();
    shared_ptr<ValueType> getValueType();
};

#endif