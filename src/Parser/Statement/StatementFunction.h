#ifndef STATEMENT_FUNCTION_H
#define STATEMENT_FUNCTION_H

#include <format>

#include "Statement.h"
#include "StatementFunctionDeclaration.h"

class StatementBlock;
class ValueType;

class StatementFunction: public Statement {
friend class Parser;

private:
    bool shouldExport;
    string name;
    string moduleName;
    vector<pair<string, shared_ptr<ValueType>>> arguments;
    shared_ptr<ValueType> returnValueType;
    shared_ptr<StatementBlock> statementBlock;

public:
    StatementFunction(
        bool shouldExport,
        const string &name,
        const vector<pair<string, shared_ptr<ValueType>>> &arguments,
        shared_ptr<ValueType> returnValueType,
        shared_ptr<StatementBlock> statementBlock,
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
    shared_ptr<StatementBlock> getStatementBlock() const;
    shared_ptr<StatementFunctionDeclaration> getDeclaration() const;
};

#endif