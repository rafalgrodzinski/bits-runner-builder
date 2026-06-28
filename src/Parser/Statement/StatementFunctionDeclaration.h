#ifndef STATEMENT_FUNCTION_DECLARATION_H
#define STATEMENT_FUNCTION_DECLARATION_H

#include <format>

#include "Statement.h"

class ValueType;

class StatementFunctionDeclaration: public Statement {
friend class Parser;

private:
    bool shouldExport;
    string name;
    string moduleName;

    vector<pair<string, shared_ptr<ValueType>>> arguments;
    shared_ptr<ValueType> returnValueType;

public:
    StatementFunctionDeclaration(
        bool shouldExport,
        const string &name,
        const string &moduleName,
        const vector<pair<string, shared_ptr<ValueType>>> &arguments,
        shared_ptr<ValueType> returnValueType,
        shared_ptr<Location> location
    );

    bool getShouldExport() const;
    string getName() const;
    string getGlobalName() const;
    string getModuleName() const;
    void setModuleName(const string &moduleName);
    vector<pair<string, shared_ptr<ValueType>>> getArguments() const;
    shared_ptr<ValueType> getReturnValueType() const;
    shared_ptr<ValueType> getValueType() const;
};

#endif