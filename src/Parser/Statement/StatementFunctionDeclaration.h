#ifndef STATEMENT_FUNCTION_DECLARATION_H
#define STATEMENT_FUNCTION_DECLARATION_H

#include "Statement.h"

class ValueType;

class StatementFunctionDeclaration: public Statement {
private:
    bool shouldExport;
    string name;
    vector<pair<string, shared_ptr<ValueType>>> arguments;
    shared_ptr<ValueType> returnValueType;

public:
    StatementFunctionDeclaration(bool shouldExport, string name, vector<pair<string, shared_ptr<ValueType>>> arguments, shared_ptr<ValueType> returnValueType);
    bool getShouldExport();
    string getName();
    vector<pair<string, shared_ptr<ValueType>>> getArguments();
    shared_ptr<ValueType> getReturnValueType();
};

#endif